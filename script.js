// Start the process by sending a request to the backend
fetch('http://localhost:3000/start', {
  method: 'POST',
})
  .then(response => response.json())
  .then(data => {
    document.getElementById('output').textContent = data.output; // Display initial output
    document.getElementById('inputSection').style.display = 'block'; // Show input form
  })
  .catch(error => {
    console.error('Error:', error);
    document.getElementById('output').textContent = 'Error loading the C++ program.';
  });

// Function to send user input to the backend and update output
function sendInput(userInput) {
  fetch('http://localhost:3000/input', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({ input: userInput }),
  })
    .then(response => response.json())
    .then(data => {
      document.getElementById('output').textContent = data.output; // Update output with new data
    })
    .catch(error => console.error('Error:', error));
}

// Attach event listener to the input button
document.getElementById('sendInputButton').addEventListener('click', () => {
  const userInput = document.getElementById('userInput').value;
  if (userInput) {
    sendInput(userInput); // Send the user input to the backend
    document.getElementById('userInput').value = ''; // Clear the input field
  }
});
