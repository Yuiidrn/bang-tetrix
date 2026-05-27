module.exports = {
  apps: [{
    name: "bang-tetrix-server",
    script: "server.js",
    instances: "max",
    exec_mode: "cluster",
    env: {
      NODE_ENV: "production",
      PORT: 3000
    },
    env_production: {
      NODE_ENV: "production"
    },
    watch: false,
    merge_logs: true,
    log_date_format: "YYYY-MM-DD HH:mm:ss Z",
    log_file: "logs/pm2/combined.log",
    error_file: "logs/pm2/error.log",
    out_file: "logs/pm2/out.log",
    max_memory_restart: "200M"
  }]
}; 