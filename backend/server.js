require('dotenv').config();
const express = require("express");

const cors = require("cors");

require("./db");

const auth = require("./auth");
const tasks = require("./tasks");
const report = require("./report");

const app = express();

app.use(cors());
app.use(express.json());


app.post("/register",auth.register);
app.post("/login",auth.login);


app.post("/tasks",tasks.addTask);
app.get("/tasks/:id",tasks.getTasks);

app.put("/tasks/:id/:taskId",tasks.completeTask);
app.put("/tasks/edit/:taskId",tasks.updateTask);

app.delete("/tasks/:taskId",tasks.deleteTask);


app.get("/schedule/:id",tasks.autoSchedule);


app.get("/report/:userId",report.generateReport);

app.listen(3000,()=>{
 console.log("Server running on 3000");
});
