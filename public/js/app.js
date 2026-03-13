document.addEventListener('DOMContentLoaded', () => {
    checkAuth();

    
    const loginForm = document.getElementById('login-form');
    if (loginForm) {
        loginForm.addEventListener('submit', handleLogin);
    }

    const regForm = document.getElementById('register-form');
    if (regForm) {
        regForm.addEventListener('submit', handleRegister);
    }

    const orderForm = document.getElementById('order-form');
    if (orderForm) {
        orderForm.addEventListener('submit', handleOrderSubmit);
    }

    const supportForm = document.getElementById('support-form');
    if (supportForm) {
        supportForm.addEventListener('submit', handleSupportSubmit);
    }

    
    if (document.getElementById('orders-list')) {
        loadProfile();
        loadOrders();
        document.getElementById('profile-search').addEventListener('input', loadOrders);
        document.getElementById('profile-status-filter').addEventListener('change', loadOrders);
    }
    if (document.getElementById('admin-orders-list')) {
        loadAllOrders();
        
        
        const adminSearch = document.getElementById('admin-search');
        const adminFilter = document.getElementById('admin-status-filter');
        if (adminSearch) adminSearch.addEventListener('input', loadAllOrders);
        if (adminFilter) adminFilter.addEventListener('change', loadAllOrders);
    }
});

function checkAuth() {
    const userJson = localStorage.getItem('user');
    const user = userJson ? JSON.parse(userJson) : null;
    const navLogin = document.getElementById('nav-login');
    const navRegister = document.getElementById('nav-register');
    const navProfile = document.getElementById('nav-profile');
    const navOrders = document.getElementById('nav-orders');
    const navAdmin = document.getElementById('nav-admin');
    const navLogout = document.getElementById('nav-logout');

    const heroPrimary = document.getElementById('hero-primary');
    const heroSecondary = document.getElementById('hero-secondary');

    if (user) {
        if (navLogin) navLogin.style.display = 'none';
        if (navRegister) navRegister.style.display = 'none';
        if (navProfile) navProfile.style.display = 'block';
        if (navAdmin) navAdmin.style.display = user.role === 'Admin' ? 'block' : 'none';
        if (navOrders) navOrders.style.display = 'block';
        if (navLogout) navLogout.style.display = 'block';

        if (heroPrimary) {
            heroPrimary.textContent = 'My Profile';
            heroPrimary.href = '/profile';
        }
        if (heroSecondary) {
            heroSecondary.textContent = 'Support';
            heroSecondary.href = '/support';
        }
    } else {
        if (navLogin) navLogin.style.display = 'block';
        if (navRegister) navRegister.style.display = 'block';
        if (navProfile) navProfile.style.display = 'none';
        if (navOrders) navOrders.style.display = 'none';
        if (navLogout) navLogout.style.display = 'none';

        if (heroPrimary) {
            heroPrimary.textContent = 'Get Started';
            heroPrimary.href = '/register';
        }
        if (heroSecondary) {
            heroSecondary.textContent = 'Learn More';
            heroSecondary.href = '/support';
        }

        const path = window.location.pathname;
        if (path === '/profile' || path === '/orders') {
            window.location.href = '/login';
        }
    }
}

function logout() {
    localStorage.removeItem('user');
    window.location.href = '/';
}

function truncateText(text, limit) {
    if (!text) return "";
    return text.length > limit ? text.substring(0, limit) + "..." : text;
}

function renderTable(tbody, data, isAdmin = false) {
    if (data.length === 0) {
        tbody.innerHTML = `<tr><td colspan="${isAdmin ? 7 : 6}" style="text-align:center;">No requests found.</td></tr>`;
        return;
    }

    
    const sortedData = [...data].sort((a, b) => b.id - a.id);

    tbody.innerHTML = sortedData.map(o => `
        <tr>
            <td>#${o.id}</td>
            ${isAdmin ? `<td><strong>${o.author}</strong></td>` : ''}
            <td>${truncateText(o.type === 'Support' ? ('Support: ' + (o.category || 'General')) : (o.service || 'Order'), 25)}</td>
            <td>${truncateText(o.description, 40)}</td>
            <td><span class="badge ${o.status.toLowerCase().replace(' ', '-')}">${o.status}</span></td>
            <td>
                ${o.type === 'Support'
            ? '-'
            : ('$' + (o.budget || '0'))
        }
            </td>
            <td>
                <div style="display:flex; gap:5px; align-items:center;">
                    ${isAdmin ? `
                        <select onchange="updateStatus(${o.id}, this.value)" style="padding: 2px; font-size: 0.8rem;">
                            <option value="">Status</option>
                            <option value="In Progress" ${o.status === 'In Progress' ? 'selected' : ''}>In Progress</option>
                            <option value="Accepted" ${o.status === 'Accepted' ? 'selected' : ''}>Accepted</option>
                            <option value="Completed" ${o.status === 'Completed' ? 'selected' : ''}>Completed</option>
                            <option value="Resolved" ${o.status === 'Resolved' ? 'selected' : ''}>Resolved</option>
                            <option value="Cancelled" ${o.status === 'Cancelled' ? 'selected' : ''}>Cancelled</option>
                        </select>
                        ${o.type === 'Support' ? `
                            <button onclick="handleRespond(${o.id})" class="btn btn-secondary" style="padding:2px 5px; font-size:0.7rem;">Respond</button>
                        ` : ''}
                    ` : ``}
                    <a href="/details.html?id=${o.id}" class="btn btn-secondary" style="padding:2px 8px; font-size:0.7rem;">Details</a>
                </div>
            </td>
        </tr>
    `).join('');
}

async function handleLogin(e) {
    e.preventDefault();
    const formData = new FormData(e.target);
    const data = Object.fromEntries(formData);

    try {
        const response = await fetch('/api/login', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        const result = await response.json();
        if (response.ok) {
            localStorage.setItem('user', JSON.stringify(result));
            window.location.href = '/profile';
        } else {
            alert(result.error || 'Login failed');
        }
    } catch (err) {
        console.error(err);
        alert('Server error');
    }
}

async function handleRegister(e) {
    e.preventDefault();
    const formData = new FormData(e.target);
    const data = Object.fromEntries(formData);

    if (!data.firstName || !data.lastName || !data.email) {
        alert('Please fill in all fields');
        return;
    }

    try {
        const response = await fetch('/api/register', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        const result = await response.json();
        if (response.ok) {
            alert('Registration successful! Please login.');
            window.location.href = '/login';
        } else {
            alert(result.error || 'Registration failed');
        }
    } catch (err) {
        console.error(err);
        alert('Server error');
    }
}

async function loadProfile() {
    const userJson = localStorage.getItem('user');
    if (!userJson) return;
    const user = JSON.parse(userJson);

    const container = document.getElementById('user-info');
    if (!container) return;

    container.innerHTML = `
        <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 1.5rem;">
            <div>
                <p style="color:var(--text-muted); font-size: 0.8rem; margin-bottom: 0.2rem;">Full Name</p>
                <h2 style="margin: 0;">${user.firstName} ${user.lastName}</h2>
                <p style="color:var(--accent); font-weight: 600;">${user.role}</p>
            </div>
            <div style="text-align: right;">
                <p style="color:var(--text-muted); font-size: 0.8rem; margin-bottom: 0.2rem;">Email</p>
                <p style="font-weight: 600;">${user.email || 'N/A'}</p>
                <p style="color:var(--text-muted); font-size: 0.8rem;">Username: ${user.username}</p>
            </div>
        </div>
    `;
}

async function loadOrders() {
    const user = JSON.parse(localStorage.getItem('user'));
    const tbody = document.getElementById('orders-list');
    if (!user || !tbody) return;

    try {
        const response = await fetch('/api/orders', {
            headers: { 'X-Username': user.username }
        });
        let orders = await response.json();

        
        const searchInput = document.getElementById('profile-search');
        const statusFilter = document.getElementById('profile-status-filter');

        const search = searchInput ? searchInput.value.toLowerCase() : '';
        const status = statusFilter ? statusFilter.value : '';

        orders = orders.filter(o => {
            const matchesSearch = o.description.toLowerCase().includes(search) || (o.service && o.service.toLowerCase().includes(search)) || (o.category && o.category.toLowerCase().includes(search));
            const matchesStatus = !status || o.status === status;
            return matchesSearch && matchesStatus;
        });

        renderTable(tbody, orders, false);

    } catch (err) {
        console.error(err);
        tbody.innerHTML = '<tr><td colspan="6">Error loading orders.</td></tr>';
    }
}

async function handleOrderSubmit(e) {
    e.preventDefault();
    const user = JSON.parse(localStorage.getItem('user'));
    if (!user) {
        alert('Must be logged in to order.');
        return;
    }

    const formData = new FormData(e.target);
    const data = Object.fromEntries(formData);
    data.username = user.username;

    try {
        const response = await fetch('/api/orders/create', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        if (response.ok) {
            alert('Order submitted successfully!');
            window.location.reload();
        } else {
            const err = await response.json();
            alert(err.error || 'Failed to submit order');
        }
    } catch (err) {
        console.error(err);
        alert('Server error');
    }
}
async function handleSupportSubmit(e) {
    e.preventDefault();
    const user = JSON.parse(localStorage.getItem('user'));
    if (!user) {
        alert('Must be logged in to submit a ticket.');
        return;
    }

    const formData = new FormData(e.target);
    const data = Object.fromEntries(formData);
    data.username = user.username;

    try {
        const response = await fetch('/api/support/create', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        if (response.ok) {
            alert('Ticket Sent! We will contact you soon.');
            e.target.reset();
        } else {
            const err = await response.json();
            alert(err.error || 'Failed to submit ticket');
        }
    } catch (err) {
        console.error(err);
        alert('Server error');
    }
}

async function loadAllOrders() {
    const user = JSON.parse(localStorage.getItem('user'));
    const tbody = document.getElementById('admin-orders-list');
    if (!user || !tbody) return;

    try {
        const response = await fetch('/api/admin/orders', {
            headers: { 'X-Username': user.username }
        });

        if (!response.ok) {
            tbody.innerHTML = '<tr><td colspan="6" style="text-align:center; color:red;">Permission Denied</td></tr>';
            return;
        }

        let orders = await response.json();

        
        const search = document.getElementById('admin-search').value.toLowerCase();
        const status = document.getElementById('admin-status-filter').value;

        orders = orders.filter(o => {
            const matchesSearch = o.description.toLowerCase().includes(search) || o.author.toLowerCase().includes(search);
            const matchesStatus = !status || o.status === status;
            return matchesSearch && matchesStatus;
        });

        renderTable(tbody, orders, true);

    } catch (err) {
        console.error(err);
        tbody.innerHTML = '<tr><td colspan="6">Error connecting to server.</td></tr>';
    }
}

async function updateStatus(orderId, newStatus) {
    if (!newStatus) return;
    const user = JSON.parse(localStorage.getItem('user'));

    try {
        const response = await fetch('/api/admin/orders/update', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-Username': user.username
            },
            body: JSON.stringify({ id: orderId, status: newStatus })
        });

        if (response.ok) {
            loadAllOrders(); 
        } else {
            alert('Failed to update status');
        }
    } catch (err) {
        console.error(err);
        alert('Server error');
    }
}

async function handleRespond(requestId) {
    const text = prompt("Enter response to support ticket:");
    if (!text) return;

    const user = JSON.parse(localStorage.getItem('user'));
    try {
        const response = await fetch('/api/admin/support/respond', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-Username': user.username
            },
            body: JSON.stringify({ id: requestId, response: text })
        });

        if (response.ok) {
            alert('Response sent and ticket resolved!');
            loadAllOrders();
        } else {
            alert('Failed to send response');
        }
    } catch (err) {
        console.error(err);
        alert('Server error');
    }
}
