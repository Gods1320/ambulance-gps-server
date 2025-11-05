import express from "express";
import mysql from "mysql2";
import cors from "cors";

const app = express();
app.use(cors());
app.use(express.json());

// ✅ Uses Railway variables
const db = mysql.createPool({
  host: process.env.MYSQLHOST,
  user: process.env.MYSQLUSER,
  password: process.env.MYSQLPASSWORD,
  database: process.env.MYSQLDATABASE,
  port: process.env.MYSQLPORT,
  ssl: {
    rejectUnauthorized: false
  },
  connectTimeout: 20000  // 20 seconds wait for Railway
});


// Test DB
db.getConnection((err) => {
  if (err) {
    console.log("❌ Database Connection Failed:", err);
  } else {
    console.log("✅ Connected to Railway Database!");
  }
});

app.get("/", (req, res) => {
  res.send("🚑 GPS Server Running");
});

// ✅ GPS Insert Endpoint (same format as ESP)
app.get("/gps_api/post_gps.php", (req, res) => {
  const { lat, lon, speed, alt, vsat, usat, acc } = req.query;

  const sql = `INSERT INTO gps_logs (latitude, longitude, speed, altitude, visible_satellites, used_satellites, accuracy)
               VALUES (?, ?, ?, ?, ?, ?, ?)`;

  db.query(sql, [lat, lon, speed, alt, vsat, usat, acc], (err) => {
    if (err) {
      console.log("❌ Insert Error:", err.sqlMessage);
      return res.status(500).send("DB Error");
    }
    console.log("✅ GPS saved:", lat, lon);
    res.send("OK");
  });
});

// Server
const port = process.env.PORT || 8080;
app.listen(port, () => console.log(`🚀 Server running on port ${port}`));
