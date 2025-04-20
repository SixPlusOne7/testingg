const express = require('express');
const { spawn } = require('child_process');
const app = express();
const port = 3000;

app.use(express.json());

app.post('/run', (req, res) => {
  const input = req.body.input; // whatever your frontend sends

  const cpp = spawn('./run_cpp_wrapper.sh', [input]);

  let output = '';
  cpp.stdout.on('data', (data) => {
    output += data.toString();
  });

  cpp.stderr.on('data', (data) => {
    console.error(`stderr: ${data}`);
  });

  cpp.on('close', (code) => {
    res.send({ result: output });
  });
});

app.listen(port, () => {
  console.log(`Server listening on http://localhost:${port}`);
});

