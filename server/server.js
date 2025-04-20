const express = require('express');
const cors = require('cors');
const { spawn } = require('child_process');
const app = express();

// Configuration
const CONFIG = {
  PORT: 3000,
  COMMAND_TIMEOUT: 5000,
  SESSION_TIMEOUT: 30 * 60 * 1000, // 30 minutes
  MAX_BUFFER_SIZE: 1024 * 1024,    // 1MB
  CPP_PROGRAM_PATH: './project'    // Changed to match your executable
};

app.use(cors());
app.use(express.json());

const activeSessions = new Map();

// Session cleanup interval
setInterval(cleanupSessions, CONFIG.SESSION_TIMEOUT / 2);

app.post('/start', (req, res) => {
  const sessionId = Date.now().toString();
  const cppProcess = spawn(CONFIG.CPP_PROGRAM_PATH, { 
    stdio: ['pipe', 'pipe', 'pipe'] 
  });

  const session = {
    process: cppProcess,
    buffer: '',
    lastActive: Date.now(),
    timeout: null
  };

  // Handle output
  cppProcess.stdout.on('data', (data) => {
    session.buffer += data.toString();
    session.lastActive = Date.now();
  });

  // Handle errors
  cppProcess.stderr.on('data', (data) => {
    console.error(`[${sessionId}] C++ Error:`, data.toString());
  });

  cppProcess.on('close', (code) => {
    console.log(`[${sessionId}] C++ process exited with code ${code}`);
    activeSessions.delete(sessionId);
  });

  activeSessions.set(sessionId, session);
  res.json({ sessionId, output: "C++ program started. Send commands to /command" });
});

app.post('/command', (req, res) => {
  const { sessionId, command } = req.body;

  if (!activeSessions.has(sessionId)) {
    return res.status(404).json({ error: "Session not found. Call /start first." });
  }

  const session = activeSessions.get(sessionId);
  session.buffer = '';
  session.lastActive = Date.now();

  // Set command timeout
  if (session.timeout) clearTimeout(session.timeout);
  session.timeout = setTimeout(() => {
    session.process.kill('SIGTERM');
    res.status(504).json({ error: "Command timeout" });
    activeSessions.delete(sessionId);
  }, CONFIG.COMMAND_TIMEOUT);

  try {
    session.process.stdin.write(command + '\n');
    
    // Check for output completion
    const checkOutput = () => {
      if (session.buffer.length > CONFIG.MAX_BUFFER_SIZE) {
        session.process.kill('SIGTERM');
        return res.status(413).json({ error: "Output buffer overflow" });
      }

      if (session.buffer.includes('Enter your choice') || 
          session.buffer.includes('Exiting program')) {
        clearTimeout(session.timeout);
        res.json({ output: session.buffer });
      } else {
        setTimeout(checkOutput, 100);
      }
    };
    
    checkOutput();
  } catch (err) {
    console.error(`[${sessionId}] Command error:`, err);
    session.process.kill('SIGTERM');
    activeSessions.delete(sessionId);
    res.status(500).json({ error: "Command processing failed" });
  }
});

app.post('/quit', (req, res) => {
  const { sessionId } = req.body;
  if (activeSessions.has(sessionId)) {
    const session = activeSessions.get(sessionId);
    session.process.stdin.write('quit\n');
    setTimeout(() => {
      session.process.kill();
      activeSessions.delete(sessionId);
    }, 500);
  }
  res.json({ output: "Session ended" });
});

// Debug endpoint
app.get('/sessions', (req, res) => {
  res.json({
    activeSessions: Array.from(activeSessions.keys()),
    totalSessions: activeSessions.size
  });
});

function cleanupSessions() {
  const now = Date.now();
  for (const [sessionId, session] of activeSessions) {
    if (now - session.lastActive > CONFIG.SESSION_TIMEOUT) {
      session.process.kill();
      activeSessions.delete(sessionId);
      console.log(`Cleaned up stale session ${sessionId}`);
    }
  }
}

app.listen(CONFIG.PORT, () => {
  console.log(`Bank System API running on port ${CONFIG.PORT}`);
  console.log(`C++ program path: ${CONFIG.CPP_PROGRAM_PATH}`);
});
