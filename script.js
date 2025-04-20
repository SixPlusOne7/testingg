// Bank Record System Frontend
let currentSessionId = null;
const API_BASE = 'http://localhost:3000';

// DOM Elements
const elements = {
    output: document.getElementById('output'),
    commandInput: document.getElementById('commandInput'),
    submitBtn: document.getElementById('submitBtn'),
    sessionStatus: document.getElementById('sessionStatus'),
    startBtn: document.getElementById('startBtn'),
    quitBtn: document.getElementById('quitBtn')
};

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    setupEventListeners();
    startSession();
});

function setupEventListeners() {
    elements.submitBtn.addEventListener('click', sendCommand);
    elements.commandInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') sendCommand();
    });
    elements.startBtn.addEventListener('click', startSession);
    elements.quitBtn.addEventListener('click', quitSession);
}

async function startSession() {
    try {
        updateUI({
            output: "Starting bank system...",
            status: "Connecting...",
            statusColor: "blue"
        });
        
        const response = await fetch(`${API_BASE}/start`);
        
        if (!response.ok) {
            throw new Error(`Server error: ${response.status}`);
        }
        
        const data = await response.json();
        currentSessionId = data.sessionId;
        
        updateUI({
            output: data.output,
            status: `Active (ID: ${currentSessionId})`,
            statusColor: "green",
            buttons: { start: true, quit: false }
        });
        
    } catch (error) {
        console.error("Session error:", error);
        updateUI({
            output: `ERROR: ${error.message}\n\nPlease ensure:\n1. Backend is running\n2. CORS is enabled\n3. Correct URL`,
            status: "Connection failed",
            statusColor: "red",
            isError: true
        });
    }
}

async function sendCommand() {
    const command = elements.commandInput.value.trim();
    if (!command || !currentSessionId) return;

    try {
        setLoadingState(true);
        
        const response = await fetch(`${API_BASE}/command`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                sessionId: currentSessionId,
                command: command
            })
        });

        if (!response.ok) {
            throw new Error(await response.text());
        }

        const data = await response.json();
        updateUI({
            output: data.output,
            commandInput: ""
        });
        
    } catch (error) {
        console.error("Command error:", error);
        updateUI({
            output: `COMMAND ERROR: ${error.message}`,
            isError: true
        });
    } finally {
        setLoadingState(false);
    }
}

async function quitSession() {
    if (!currentSessionId) return;
    
    try {
        await fetch(`${API_BASE}/quit`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ sessionId: currentSessionId })
        });
        
        updateUI({
            output: "Session ended successfully",
            status: "Disconnected",
            statusColor: "gray",
            buttons: { start: false, quit: true }
        });
        
        currentSessionId = null;
    } catch (error) {
        console.error("Quit error:", error);
        updateUI({
            output: `ERROR: Couldn't end session properly`,
            isError: true
        });
    }
}

// UI Utilities
function updateUI({ output, status, statusColor, commandInput, isError, buttons }) {
    if (output !== undefined) {
        elements.output.innerHTML = output.replace(/\n/g, '<br>');
        elements.output.className = isError ? 'error' : '';
        elements.output.scrollTop = elements.output.scrollHeight;
    }
    if (status !== undefined) {
        elements.sessionStatus.textContent = status;
        elements.sessionStatus.style.color = statusColor || 'black';
    }
    if (commandInput !== undefined) {
        elements.commandInput.value = commandInput;
    }
    if (buttons) {
        elements.startBtn.disabled = buttons.start !== false;
        elements.quitBtn.disabled = buttons.quit !== false;
    }
}

function setLoadingState(isLoading) {
    elements.commandInput.disabled = isLoading;
    elements.submitBtn.disabled = isLoading;
    elements.submitBtn.textContent = isLoading ? '...' : 'Send';
}

// Clean up on page exit
window.addEventListener('beforeunload', quitSession);
