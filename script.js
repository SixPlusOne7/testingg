document.addEventListener('DOMContentLoaded', () => {
  const displayData = (data) => {
    const container = document.createElement('div');
    container.innerHTML = `
      <h2>${data.message}</h2>
      <p>Server Time: ${data.timestamp}</p>
      <ul>
        ${data.data.map(item => `<li>${item}</li>`).join('')}
      </ul>
    `;
    document.body.appendChild(container);
  };

  fetch('/api/data')
    .then(response => {
      if (!response.ok) throw new Error('Network error');
      return response.json();
    })
    .then(displayData)
    .catch(error => {
      console.error('Error:', error);
      document.body.innerHTML = `<p style="color:red">Error: ${error.message}</p>`;
    });
});
