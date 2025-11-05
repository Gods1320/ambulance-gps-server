// index.js
import express from "express";
import mysql from "mysql2";
import cors from "cors";

const app = express();
app.use(cors());
app.use(express.json());

const db = mysql.createPool({
  host: process.env.MYSQLHOST,          // e.g. hopper.proxy.rlwy.net
  user: process.env.MYSQLUSER,          // e.g. root
  password: process.env.MYSQLPASSWORD,  // from Railway
  database: process.env.MYSQLDATABASE,  // usually "railway"
  port: Number(process.env.MYSQLPORT),  // e.g. 16071
  // ---- TLS settings that work on Railway ----
  // Option 1 (recommended): empty object enables TLS with system CAs
  ssl: {},
  // If you STILL see TLS errors, swap to:
  // ssl: { rejectUnauthorized: false },

  // Connection stability
  enableKeepAlive: true,
  keepAliveInitialDelay: 0,
  waitForConnections: true,
  connectionLimit: 5,
  queueLimit: 0,
  connectTimeout: 20000
});

function testDbConnection(retries = 8) {
  return new Promise((resolve, reject) => {
    const attempt = (left) => {
      db.getConnection((err, conn) => {
        if (!err && conn) {
          conn.ping(() => conn.release());
          console.log("✅ Connected to Railway Database!");
          return resolve();
        }
        console.log(
          `❌ Database Connection Failed${err ? ": " + err.message : ""}`
        );
        if (left <= 0) return reject(err || new Error("DB connect failed"));
        setTimeout(() => attempt(left - 1), 3000);
      });
    };
    attempt(retries);
  });
}

app.get("/", (_req, res) => {
  res.send("🚑 GPS Server Running");
});

// Your ESP32 calls this endpoint via HTTP GET with query params
app.get("/gps_api/post_gps.php", (req, res) => {
  const { lat, lon, speed, alt, vsat, usat, acc } = req.query;

  if (
    lat === undefined || lon === undefined ||
    speed === undefined || alt === undefined ||
    vsat === undefined || usat === undefined || acc === undefined
  ) {
    return res.status(400).send("Missing params");
  }

  const sql = `
    INSERT INTO gps_logs
      (latitude, longitude, speed, altitude, visible_satellites, used_satellites, accuracy)
    VALUES (?, ?, ?, ?, ?, ?, ?)
  `;

  db.query(
    sql,
    [Number(lat), Number(lon), Number(speed), Number(alt), Number(vsat), Number(usat), Number(acc)],
    (err) => {
      if (err) {
        console.log("❌ DB Insert Error:", err.message);
        return res.status(500).send("DB Error");
      }
      console.log("✅ New GPS Log Saved:", { lat, lon, speed, alt, vsat, usat, acc });
      res.send("OK");
    }
  );
});

const port = process.env.PORT || 8080;
app.listen(port, async () => {
  console.log(`🚀 Server running on port ${port}`);
  try {
    await testDbConnection();
  } catch (e) {
    // Keep the server up so you can see logs, but DB is not ready yet
  }
});
