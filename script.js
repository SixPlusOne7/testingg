let currentSessionId = null;

// DOM elements
const outputEl = document.getElementById('output');
const commandInput = document.getElementById('commandInput');
const submitBtn = document.getElementById('submitBtn');
const sessionStatus = document.getElementById('sessionStatus');

// Initialize session when page loads
window.addEventListener('DOMContentLoaded', startSession);

// Handle command submission
submitBtn.addEventListener('click', sendCommand);
commandInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') sendCommand();
});

async function startSession() {
    try {
        outputEl.textContent = "Starting C++ program...";
        const response = await fetch('http://localhost:3000/start');
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const data = await response.json();
        currentSessionId = data.sessionId;
        sessionStatus.textContent = `Active (ID: ${currentSessionId})`;
        updateOutput(data.output);
    } catch (error) {
        console.error('Error starting session:', error);
        outputEl.textContent = `Error: ${error.message}`;
    }
}

async function sendCommand() {
    if (!currentSessionId) {
        updateOutput("Error: No active session. Please refresh the page.");
        return;
    }

    const command = commandInput.value.trim();
    if (!command) return;

    try {
        // Disable input while processing
        commandInput.disabled = true;
        submitBtn.disabled = true;
        
        const response = await fetch('http://localhost:3000/command', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ 
                sessionId: currentSessionId, 
                command: command 
            })
        });

        if (!response.ok) {
            throw new Error(`Command failed: ${response.status}`);
        }

        const data = await response.json();
        updateOutput(data.output);
        commandInput.value = ''; // Clear input
        
    } catch (error) {
        console.error('Error sending command:', error);
        updateOutput(`Error: ${error.message}`);
    } finally {
        commandInput.disabled = false;
        submitBtn.disabled = false;
        commandInput.focus();
    }
}

function updateOutput(text) {
    outputEl.textContent = text;
    outputEl.scrollTop = outputEl.scrollHeight; // Auto-scroll to bottom
}

// Clean up session when page closes
window.addEventListener('beforeunload', async () => {
    if (currentSessionId) {
        try {
            await fetch('http://localhost:3000/quit', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ sessionId: currentSessionId })
            });
        } catch (e) {
            console.log('Error quitting session:', e);
        }
    }
});
