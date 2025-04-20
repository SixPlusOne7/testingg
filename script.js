k// Store the session ID globally
let currentSessionId = null;

async function startProgram() {
    const response = await fetch('http://localhost:3000/start');
    const data = await response.json();
    currentSessionId = data.sessionId;
    updateOutput(data.output);
}

async function sendCommand(command) {
    if (!currentSessionId) {
        alert('No active session. Start first!');
        return;
    }
    
    const response = await fetch('http://localhost:3000/command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
            sessionId: currentSessionId,
            command: command
        })
    });
    const data = await response.json();
    updateOutput(data.output);
}

async function quitProgram() {
    if (currentSessionId) {
        await fetch('http://localhost:3000/quit', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ sessionId: currentSessionId })
        });
        currentSessionId = null;
    }
}
