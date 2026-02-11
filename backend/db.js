const mongoose = require("mongoose");


mongoose.connect(process.env.MONGO_URI)
  .then(() => console.log("Cloud MongoDB Connected via .env..."))
  .catch(err => console.log("Connection Error: ", err));

module.exports = mongoose;