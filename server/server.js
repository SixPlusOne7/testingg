const express = require('express');
const path = require('path');
const cors = require('cors');
const app = express();

// Middleware setup
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, '..'))); // Serve files from root

// API endpoint
app.get('/api/data', (req, res) => {
  res.json({
    message: 'API is working!',
    timestamp: new Date().toISOString(),
    data: [10, 20, 30]
  });
});

// Fallback to index.html for client-side routing
app.get('*', (req, res) => {
  res.sendFile(path.join(__dirname, '../index.html'));
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
