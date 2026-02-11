// ================= GLOBAL CONFIG =================
const API = "https://smart-task-scheduler-lp72.onrender.com";
let undoStack = [];
let redoStack = [];

// ================= REGISTER (FOR REGISTER.HTML) =================
async function doRegister() {
    try {
        // IDs must match register.html: ruser and rpass
        const username = document.getElementById("ruser").value;
        const password = document.getElementById("rpass").value;

        if (!username || !password) {
            alert("Please fill up the records");
            return;
        }

        const res = await fetch(`${API}/register`, {
            method: "POST",
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username, password })
        });

        if (res.ok) {
            alert("Registered Successfully");
            window.location.href = "login.html";
        } else {
            const errorMsg = await res.text();
            alert("Error: " + errorMsg);
        }
    } catch (err) {
        console.error("Register Error:", err);
        alert("Backend server off ");
    }
}

// ================= LOGIN (FOR LOGIN.HTML) =================
async function login() {
    try {
        const username = document.getElementById("user").value;
        const password = document.getElementById("pass").value;

        const res = await fetch(`${API}/login`, {
            method: "POST",
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username, password })
        });

        const data = await res.json();

        if (!data.id) {
            alert("Invalid Credentials!");
            return;
        }

        localStorage.id = data.id;
        localStorage.token = data.token; // Saving JWT
        window.location.href = "dashboard.html";
    } catch (err) {
        alert("Login failed!");
    }
}

// ================= TASK MANAGEMENT (FOR DASHBOARD.HTML) =================
async function loadTasks() {
    if (!localStorage.id) return;
    try {
        const res = await fetch(`${API}/tasks/${localStorage.id}`);
        const data = await res.json();
        
        let html = "";
        data.forEach(t => {
            const isOverdue = new Date(t.deadline) < new Date() && !t.completed;
            html += `
            <div class="group bg-white/5 border border-white/10 p-4 rounded-xl flex justify-between items-center transition shadow-lg">
                <div class="flex-1">
                    <b class="${t.completed ? 'line-through text-slate-500' : 'text-slate-100'}">${t.title}</b>
                    <p class="text-sm text-slate-400 mt-1">${t.category} | ${new Date(t.deadline).toLocaleDateString()}</p>
                </div>
                <div class="flex gap-4">
                    <button onclick="openEdit('${t._id}','${t.title}','${t.description}','${t.category}','${t.deadline}','${t.priority}','${t.dependencies}')" class="text-blue-400">✏</button>
                    ${t.completed ? '✅' : `<button onclick="completeTask('${t._id}')" class="text-green-400">✔</button>`}
                    <button onclick="deleteTask('${t._id}')" class="text-red-400">✖</button>
                </div>
            </div>`;
        });
        document.getElementById("taskList").innerHTML = html || '<p class="text-center text-slate-500">No tasks yet.</p>';
    } catch (e) { console.log(e); }
}

// Add Task with Validation
document.getElementById("taskForm")?.addEventListener("submit", async (e) => {
    e.preventDefault();
    const taskData = {
        userId: localStorage.id,
        title: document.getElementById("title").value,
        description: document.getElementById("desc").value,
        category: document.getElementById("category").value,
        deadline: document.getElementById("deadline").value,
        priority: parseInt(document.getElementById("priority").value) || 3,
        dependencies: document.getElementById("depend").value
    };

    await fetch(`${API}/tasks`, {
        method: "POST",
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(taskData)
    });
    e.target.reset();
    loadTasks();
});

async function completeTask(taskId) {
    await fetch(`${API}/tasks/${localStorage.id}/${taskId}`, { method: "PUT" });
    loadTasks();
}

async function deleteTask(taskId) {
    await fetch(`${API}/tasks/${taskId}`, { method: "DELETE" });
    loadTasks();
}

// ================= ANALYTICS & REPORTS =================
function downloadReport() {
    window.open(`${API}/report/${localStorage.id}`);
}

async function openSchedule() {
    const res = await fetch(`${API}/schedule/${localStorage.id}`);
    const data = await res.json();
    alert("AI Schedule: " + data.map(t => t.title).join(", "));
}

function logout() {
    localStorage.clear();
    window.location.href = "login.html";
}

// Initializing based on page
window.onload = () => {
    if (window.location.pathname.includes("dashboard")) loadTasks();

};
