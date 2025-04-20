const express = require('express');
const cors = require('cors');  // Add CORS support
const { spawn } = require('child_process');
const app = express();

app.use(cors());
app.use(express.json());

// Global config
const COMMAND_TIMEOUT = 5000;
const MAX_BUFFER_SIZE = 1024 * 1024;
const activeUsers = new Map();

// C++ program path (absolute path recommended)
const CPP_PROGRAM_PATH = './cpp_program';

app.post('/command', (req, res) => {
  const { sessionId, command } = req.body;
  
  // Initialize session
  if (!activeUsers.has(sessionId)) {
    activeUsers.set(sessionId, {
      buffer: '',
      lastActive: Date.now(),
      cppProcess: spawn(CPP_PROGRAM_PATH)
    });
  }

  const userSession = activeUsers.get(sessionId);
  userSession.lastActive = Date.now();

  // Timeout handler
  const timeoutId = setTimeout(() => {
    userSession.cppProcess.kill('SIGTERM');
    res.status(504).json({ error: "Command timeout" });
  }, COMMAND_TIMEOUT);

  // Process output
  userSession.cppProcess.stdout.on('data', (data) => {
    if (userSession.buffer.length + data.length > MAX_BUFFER_SIZE) {
      userSession.cppProcess.kill('SIGTERM');
      return res.status(413).json({ error: "Buffer overflow" });
    }
    userSession.buffer += data.toString();  // Convert Buffer to string
  });

  // Send command to C++ program
  userSession.cppProcess.stdin.write(command + '\n');

  // Check for output completion
  const checkOutput = () => {
    if (userSession.buffer.includes('Enter command')) {  // Detect C++ program prompt
      clearTimeout(timeoutId);
      res.json({ output: userSession.buffer });
      userSession.buffer = '';
    } else {
      setTimeout(checkOutput, 100);
    }
  };
  checkOutput();
});

app.listen(3000, () => console.log('Bank System API running on port 3000'));
