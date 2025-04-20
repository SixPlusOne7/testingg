const express = require('express');
const { spawn } = require('child_process');
const app = express();
app.use(express.json());

// 全局配置
const COMMAND_TIMEOUT = 5000; // 5秒超时
const MAX_BUFFER_SIZE = 1024 * 1024; // 1MB限制
const activeUsers = new Map(); // 会话管理

// 原有处理逻辑核心部分
app.post('/command', (req, res) => {
  const { sessionId, command } = req.body;
  
  // 会话初始化
  if (!activeUsers.has(sessionId)) {
    activeUsers.set(sessionId, {
      buffer: '',
      lastActive: Date.now(),
      cppProcess: spawn('./cpp_program')
    });
  }

  const userSession = activeUsers.get(sessionId);
  userSession.lastActive = Date.now();

  // 超时控制
  const timeoutId = setTimeout(() => {
    userSession.cppProcess.kill('SIGTERM');
    res.status(504).json({ error: "Command timeout" });
  }, COMMAND_TIMEOUT);

  // 原有输出处理逻辑
  userSession.cppProcess.stdout.on('data', (data) => {
    if (userSession.buffer.length + data.length > MAX_BUFFER_SIZE) {
      userSession.cppProcess.kill('SIGTERM');
      return res.status(413).json({ error: "Buffer overflow" });
    }
    userSession.buffer += data;
  });

  // 命令执行
  userSession.cppProcess.stdin.write(command + '\n');

  // 改进的输出检查机制
  const checkOutput = () => {
    if (/* 检测输出终止条件 */) {
      clearTimeout(timeoutId);
      res.json({ output: userSession.buffer });
      userSession.buffer = ''; // 清空缓冲区
    } else {
      setTimeout(checkOutput, 100);
    }
  };
  checkOutput();
});

// 进程终止接口（含原有逻辑改进）
app.post('/quit', (req, res) => {
  const { sessionId } = req.body;
  const userSession = activeUsers.get(sessionId);

  if (userSession?.cppProcess) {
    userSession.cppProcess.once('exit', () => {
      activeUsers.delete(sessionId);
      res.json({ status: "Process terminated" });
    });
    userSession.cppProcess.kill('SIGTERM');
  } else {
    res.status(410).json({ error: "No active process" });
  }
});

// 新增错误处理（原有逻辑缺失部分）
process.on('unhandledRejection', (err) => {
  console.error('Unhandled rejection:', err);
  activeUsers.forEach(session => {
    session.cppProcess?.kill('SIGTERM');
  });
});

app.listen(3000, () => console.log('Server running on port 3000'));
