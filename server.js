import express from "express";
import mysql from "mysql2";
import cors from "cors";

const app = express();
app.use(cors());
app.use(express.json());

const db = mysql.createPool({
  host: process.env.MYSQLHOST,
  user: process.env.MYSQLUSER,
  password: process.env.MYSQLPASSWORD,
  database: process.env.MYSQLDATABASE,
  port: process.env.MYSQLPORT
});

db.getConnection((err) => {
  if (err) return console.log("❌ Database Connection Failed:", err);
  console.log("✅ Connected to Railway Database!");
});

app.get("/", (req, res) => {
  res.send("🚑 GPS Server Running on Railway");
});

app.get("/gps_api/post_gps.php", (req, res) => {
  const { lat, lon, speed, alt, vsat, usat, acc } = req.query;

  const sql = `INSERT INTO gps_logs (latitude, longitude, speed, altitude, visible_satellites, used_satellites, accuracy) VALUES (?, ?, ?, ?, ?, ?, ?)`;

  db.query(sql, [lat, lon, speed, alt, vsat, usat, acc], (err) => {
    if (err) {
      console.log("❌ DB Insert Error:", err);
      return res.status(500).send("DB Error");
    }
    console.log("✅ GPS Stored:", lat, lon);
    res.send("OK");
  });
});

const port = process.env.PORT || 3000;
app.listen(port, () => console.log(`🚀 Server running on :${port}`));
