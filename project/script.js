let updateInterval = null;
let currentPlayer = 1;
let myPlayerType = null;

async function apiCall(endpoint, method = 'GET', data = null) {
    const options = {
        method: method,
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        }
    };
    
    if (data) {
        options.body = new URLSearchParams(data).toString();
    }
    
    try {
        const response = await fetch(endpoint, options);
        return await response.json();
    } catch (error) {
        console.error('API Error:', error);
        showToast('Ошибка соединения с сервером');
        return null;
    }
}

async function updateGameState() {
    const state = await apiCall('/api/game_state');
    if (state) {
        renderBoard(state);
        updateUI(state);
    }
}

async function handleCellClick(row, col) {
    const state = await apiCall('/api/game_state');
    if (!state || state.gameOver) return;
    
    if (state.pieceSelected) {
        const result = await apiCall('/api/move_selected', 'POST', { row: row, col: col });
        if (result && result.success) {
            await updateGameState();
        }
    } else {
        const piece = state.board[row][col];
        if (piece !== 0) {
            const isCorrectPlayer = (piece === 1 || piece === 3) ? state.currentPlayer === 1 : state.currentPlayer === 2;
            if (isCorrectPlayer) {
                await apiCall('/api/select', 'POST', { row: row, col: col });
                await updateGameState();
            } else {
                showToast('Сейчас ход другого игрока!');
            }
        }
    }
}

async function resetGame() {
    await apiCall('/api/reset', 'POST');
    await updateGameState();
    showToast('Игра перезапущена!');
}

function renderBoard(state) {
    const boardElement = document.getElementById('board');
    boardElement.innerHTML = '';
    
    for (let i = 0; i < 8; i++) {
        for (let j = 0; j < 8; j++) {
            const cell = document.createElement('div');
            const isDark = (i + j) % 2 === 1;
            cell.className = `cell ${isDark ? 'black' : 'white'}`;
            
            if (state.selectedRow === i && state.selectedCol === j) {
                cell.classList.add('selected');
            }
            
            // Проверка возможных ходов (упрощенно)
            if (state.pieceSelected && state.selectedRow === i && state.selectedCol === j) {
                cell.classList.add('possible-move');
            }
            
            const piece = state.board[i][j];
            if (piece !== 0) {
                const pieceDiv = document.createElement('div');
                pieceDiv.className = `piece ${piece === 1 || piece === 3 ? 'white-piece' : 'black-piece'}`;
                if (piece === 3 || piece === 4) {
                    pieceDiv.classList.add('king');
                }
                cell.appendChild(pieceDiv);
            }
            
            cell.onclick = (function(row, col) {
                return function() { handleCellClick(row, col); };
            })(i, j);
            
            boardElement.appendChild(cell);
        }
    }
}

function updateUI(state) {
    const whitePlayer = document.getElementById('whitePlayer');
    const blackPlayer = document.getElementById('blackPlayer');
    const gameStatus = document.getElementById('gameStatus');
    
    if (state.currentPlayer === 1) {
        whitePlayer.classList.add('active');
        blackPlayer.classList.remove('active');
    } else {
        blackPlayer.classList.add('active');
        whitePlayer.classList.remove('active');
    }
    
    if (state.gameOver) {
        const winner = state.winner;
        if (winner === 1) {
            gameStatus.innerHTML = '🏆 Победили белые! 🏆';
            gameStatus.style.background = '#d4edda';
        } else if (winner === 2) {
            gameStatus.innerHTML = '🏆 Победили черные! 🏆';
            gameStatus.style.background = '#d4edda';
        } else {
            gameStatus.innerHTML = '🎮 Игра окончена';
        }
        showToast(gameStatus.innerHTML);
    } else {
        gameStatus.innerHTML = state.currentPlayer === 1 ? '🎮 Ход белых' : '🎮 Ход черных';
        gameStatus.style.background = '#f0f0f0';
    }
}

function showToast(message) {
    const toast = document.getElementById('toast');
    toast.textContent = message;
    toast.classList.add('show');
    setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

// Запуск обновления
async function init() {
    await updateGameState();
    updateInterval = setInterval(updateGameState, 500);
}

init();