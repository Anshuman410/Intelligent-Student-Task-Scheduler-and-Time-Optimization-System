const mongoose = require("mongoose");
const bcrypt = require("bcryptjs");
const jwt = require("jsonwebtoken");


const userSchema = new mongoose.Schema({
  username: { 
    type: String, 
    required: true, 
    unique: true, 
    trim: true 
  },
  password: { 
    type: String, 
    required: true 
  },
  points: { 
    type: Number, 
    default: 0 
  }
});

const User = mongoose.model("User", userSchema);


exports.register = async (req, res) => {
  try {
    const { username, password } = req.body;
    
    const existingUser = await User.findOne({ username });
    if (existingUser) return res.status(400).send("User already exists");

    const hash = await bcrypt.hash(password, 10);

    await User.create({
      username: username,
      password: hash,
      points: 0
    });

    res.status(201).send("Registered");
  } catch (e) {
    res.status(500).send("Error in Registration");
  }
};


exports.login = async (req, res) => {
  try {
    const user = await User.findOne({ username: req.body.username });

    if (!user) return res.status(404).json({ message: "Invalid User" });

    const ok = await bcrypt.compare(req.body.password, user.password);

    if (!ok) return res.status(401).json({ message: "Invalid Password" });

    const token = jwt.sign(
      { id: user._id }, 
      process.env.JWT_SECRET, 
      { expiresIn: '24h' }    
    );

    res.json({ token, id: user._id });
  } catch (e) {
    res.status(500).send("Login Error");
  }
};