const defaults = { focusMinutes: 45, sitMinutes: 45, demoMode: true, paused: false, muted: false, count: 0, level: 1 };
const saved = JSON.parse(localStorage.getItem('focus-reminder-settings') || '{}');
const state = { ...defaults, ...saved, remaining: 0, reminderType: null };
const $ = (id) => document.getElementById(id);
let interval;

function duration() { return state.demoMode ? 10 : state.focusMinutes * 60; }
function format(seconds) { return `${String(Math.floor(seconds / 60)).padStart(2, '0')}:${String(seconds % 60).padStart(2, '0')}`; }
function save() { localStorage.setItem('focus-reminder-settings', JSON.stringify({ ...state, remaining: undefined, reminderType: undefined })); }
function render() {
  $('timer').textContent = format(Math.max(0, state.remaining));
  $('statusText').textContent = state.paused ? '提醒已暂停' : state.reminderType ? '需要活动一下' : '正在专注';
  $('statusDot').classList.toggle('alert', Boolean(state.reminderType));
  $('todayCount').textContent = `今日 ${state.count} 次起身`;
  $('level').textContent = `提醒级别 ${state.level} / 3`;
  $('pauseText').textContent = state.paused ? '继续提醒' : '暂停提醒'; $('pauseIcon').textContent = state.paused ? '▶' : 'Ⅱ';
  $('muteText').textContent = state.muted ? '开启声音' : '静音'; $('muteIcon').textContent = state.muted ? '♩' : '♪';
  $('timerHint').textContent = state.paused ? '休息中，准备好后继续' : state.reminderType ? '完成后点击“我起来了”' : '专注进行中，保持这个节奏';
  $('alertPanel').classList.toggle('hidden', !state.reminderType);
  if (state.reminderType) { $('alertTitle').textContent = state.level > 1 ? '我还在等你起身' : '该活动一下了'; $('alertMessage').textContent = state.level > 1 ? '刚才的提醒被忽略了，走两分钟再回来。' : '坐了有一阵，起来走两分钟吧。'; }
}
function trigger() { if (state.paused) return; state.reminderType = 'sit'; state.level = Math.min(3, state.level + 1); render(); if (!state.muted) { try { new Audio('data:audio/wav;base64,UklGRiQAAABXQVZFZm10IBAAAAABAAEAQB8AAIA+AAACABAAZGF0YQAAAAA=').play(); } catch {} } }
function reset() { state.remaining = duration(); state.reminderType = null; render(); save(); }
function tick() { if (state.paused || state.reminderType) return; state.remaining -= 1; if (state.remaining <= 0) trigger(); else render(); }

$('respond').onclick = () => { state.count += 1; state.level = 1; reset(); };
$('snooze').onclick = () => { state.remaining = state.demoMode ? 10 : 5 * 60; state.reminderType = null; render(); save(); };
$('pause').onclick = () => { state.paused = !state.paused; render(); save(); };
$('mute').onclick = () => { state.muted = !state.muted; render(); save(); };
$('settings').onclick = () => $('settingsPanel').classList.toggle('hidden');
$('saveSettings').onclick = () => { state.focusMinutes = Math.max(1, Number($('focusMinutes').value) || 45); state.sitMinutes = Math.max(1, Number($('sitMinutes').value) || 45); state.demoMode = $('demoMode').checked; $('settingsPanel').classList.add('hidden'); reset(); };
$('minimize').onclick = () => window.desktopWindow.minimize(); $('close').onclick = () => window.desktopWindow.close();

$('focusMinutes').value = state.focusMinutes; $('sitMinutes').value = state.sitMinutes; $('demoMode').checked = state.demoMode;
state.remaining = duration(); render(); interval = setInterval(tick, 1000);
