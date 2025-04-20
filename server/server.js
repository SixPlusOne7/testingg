const express = require('express');
const { spawn } = require('child_process');
const app = express();
const cors = require('cors');

app.use(cors());
app.use(express.json());

const activeUsers = new Map();

// Add this endpoint to check active sessions
app.get('/sessions', (req, res) => {
    res.json({
        activeSessions: Array.from(activeUsers.keys()),
        totalSessions: activeUsers.size
    });
});

app.post('/start', (req, res) => {
    const sessionId = Date.now().toString();
    const cppProcess = spawn('./project', { stdio: ['pipe', 'pipe', 'pipe'] });
    
    const userSession = {
        process: cppProcess,
        buffer: '',
        lastActive: Date.now()
    };
    
    cppProcess.stdout.on('data', (data) => {
        userSession.buffer += data.toString();
        userSession.lastActive = Date.now();
    });
    
    activeUsers.set(sessionId, userSession);
    
    // Auto-cleanup after 30 minutes
    setTimeout(() => {
        if (activeUsers.has(sessionId)) {
            activeUsers.get(sessionId).process.kill();
            activeUsers.delete(sessionId);
        }
    }, 30 * 60 * 1000);
    
    res.json({ sessionId, output: "C++ program started. Send commands to /command" });
});

app.post('/command', (req, res) => {
    const { sessionId, command } = req.body;
    
    if (!activeUsers.has(sessionId)) {
        return res.status(404).json({ error: "Session not found" });
    }
    
    const userSession = activeUsers.get(sessionId);
    userSession.buffer = '';
    userSession.process.stdin.write(command + '\n');
    
    // Wait for prompt or timeout
    const checkOutput = () => {
        if (userSession.buffer.includes('Enter your choice') || 
            userSession.buffer.includes('Exiting program') ||
            userSession.buffer.length > 0) {
            res.json({ output: userSession.buffer });
        } else {
            setTimeout(checkOutput, 100);
        }
    };
    
    setTimeout(checkOutput, 100);
});

app.post('/quit', (req, res) => {
    const { sessionId } = req.body;
    if (activeUsers.has(sessionId)) {
        activeUsers.get(sessionId).process.stdin.write('quit\n');
        setTimeout(() => {
            activeUsers.get(sessionId).process.kill();
            activeUsers.delete(sessionId);
        }, 500);
    }
    res.json({ output: "Session ended" });
});

const PORT = 3000;
app.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`);
    console.log(`Active sessions available at http://localhost:${PORT}/sessions`);
});
