import express from "express";
import mysql from "mysql2";
import cors from "cors";

const app = express();
app.use(cors());
app.use(express.json());

// ✅ Use Railway ENV Variables (No Hardcoding!)
const db = mysql.createPool({
  host: process.env.MYSQLHOST,
  user: process.env.MYSQLUSER,
  password: process.env.MYSQLPASSWORD,
  database: process.env.MYSQLDATABASE,
  port: process.env.MYSQLPORT,
  ssl: {
    rejectUnauthorized: false
  }
});

// ✅ Test DB Connection
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

// ✅ POST GPS Endpoint
app.get("/gps_api/post_gps.php", (req, res) => {
  const { lat, lon, speed, alt, vsat, usat, acc } = req.query;

  const sql = `
    INSERT INTO gps_logs 
    (latitude, longitude, speed, altitude, visible_satellites, used_satellites, accuracy)
    VALUES (?, ?, ?, ?, ?, ?, ?)
  `;

  db.query(sql, [lat, lon, speed, alt, vsat, usat, acc], (err) => {
    if (err) {
      console.log("❌ DB Insert Error:", err);
      return res.status(500).send("DB Error");
    }
    console.log("✅ GPS Saved:", lat, lon);
    res.send("OK");
  });
});

const PORT = process.env.PORT || 8080;
app.listen(PORT, () => console.log(`🚀 Server running on port ${PORT}`));
