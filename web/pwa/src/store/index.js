import Vue from 'vue'
import Vuex from 'vuex'
import 'babel-polyfill'
import 'fetch-polyfill'

Vue.use(Vuex)

const power = [
  [0, 0], // 0
  [40, 210], // 0.16
  [40, 160], // 0.2
  [60, 140], // 0.3
  [80, 120], // 0.4
  [80, 80], // 0.5
  [120, 80], // 0.6
  [140, 60], // 0.7
  [160, 40], // 0.8
  [210, 40], // 0.84
  [100, 0] // 1
]

// let ip1, ip2
try {
  var { ip1, ip2 } = JSON.parse(localStorage.getItem('serverIPs'))
} catch (e) {
  ip1 = '192.168.0.177'
  ip2 = '192.168.0.209'
}

const store = new Vuex.Store({
  state: {
    count: 0,
    tempWarmer: 0,
    fan0: 0,
    fan1: 0,
    tempIn: 0,
    ssr1: 0,
    ssr2: 0,
    tempOut: 0,
    outH: '',
    outP: '',
    outT: '',
    time: getTime(),
    date: getDate(),
    preset1: loadPresets(0),
    preset2: loadPresets(1),
    preset3: loadPresets(2),
    presetBlink: false,
    ip1: ip1,
    ip2: ip2,
    inProcess: false,
    isLoading: false
  },
  mutations: {
    warmerInc (state) {
      state.tempWarmer++
      updateWarmerTemp(state.ip1, state.tempWarmer)
    },
    warmerDec (state) {
      state.tempWarmer--
      updateWarmerTemp(state.ip1, state.tempWarmer)
    },
    warmerSet (state, p) {
      state.tempWarmer = Math.floor(p[0].target_t)
    },
    fanSet (state, p) {
      state.fan0 = p[0]
      state.fan1 = p[1]
    },
    fanInc (state, id) {
      state[id]++
      updateFanSpeed(state.ip1, id, state[id])
    },
    fanDec (state, id) {
      state[id]--
      updateFanSpeed(state.ip1, id, state[id])
    },
    tempSet (state, p) {
      state.tempOut = Math.round(p[0].t)
      state.ssr1 = Math.round(p[1].t)
      state.ssr2 = Math.round(p[2].t)
      state.tempIn = Math.round(p[3].t)
    },
    outdoorSet (state, p) {
      state.outH = Math.round(p.temp.h) + '%'
      state.outT = (p.temp.t > 0 ? '+' : '') + Math.round(p.temp.t) + '°C'
      state.outP = Math.round(p.baro.p) + 'mm'
    },
    updateTime (state) {
      state.time = getTime()
      state.date = getDate()
    },
    onPresetKey (state, dataKey) {
      console.log('load preset')
      let d = state[dataKey]
      state.fan0 = d.fan0
      state.fan1 = d.fan1
      state.tempWarmer = d.tempWarmer
      updateWarmerTemp(state.ip1, state.tempWarmer)
      updateFanSpeed(state.ip1, 'fan0', state.fan0)
      updateFanSpeed(state.ip1, 'fan1', state.fan1)
    },
    toPreset (state, dataKey) {
      console.log('store preset')
      let d = state[dataKey]
      d.fan0 = state.fan0
      d.fan1 = state.fan1
      d.tempWarmer = state.tempWarmer
      storePresets(state)
    },
    presetBlinkOn (state) {
      store.state.presetBlink = true
    },
    presetBlinkOff (state) {
      store.state.presetBlink = false
    },
    setIPs (state, p) {
      store.state.ip1 = p.ip1
      store.state.ip2 = p.ip2
      localStorage.setItem('serverIPs', JSON.stringify(p))
    },
    setInPorcess (state, value) {
      store.state.inProcess = value
    },
    setIsLoading (state, value) {
      store.state.isLoading = value
    }
  },
  actions: {
    read ({ state, commit }) {
      store.commit('setIsLoading', true)
      Promise.all([
        fetch(`http://${state.ip1}/`).then(r => {
          r.json().then(r => {
            let fans = r.filter(e => e.type === 'fan').map(getPowerLevel)
            let warmer = r.filter(e => e.type === 'warmer')
            let temps = r.filter(e => e.type === 't_sensor')

            commit('warmerSet', warmer)
            commit('fanSet', fans)
            commit('tempSet', temps)
          }).catch(err => {
            console.log('parsing error 1', err)
          })
        }),
        fetch(`http://${state.ip2}/`).then(r => {
          r.json().then(r => {
            let temp = r.filter(e => e.type === 't_sensor22')
            let baro = r.filter(e => e.type === 'barometr')

            commit('outdoorSet', { temp: temp[0], baro: baro[0] })
          }).catch(err => {
            console.log('parsing error 2', err)
          })
        })
      ]).catch(err => {
        console.log('fetch err', err)
      }).then(() => {
        store.commit('setIsLoading', false)
      })
    }
  }
})
global.store = store

function getTime () {
  return new Date().toTimeString().slice(0, 5)
}
function getDate () {
  return new Date().toDateString().slice(4, 10)
}

function getPowerLevel (a) {
  for (let i = 0; i < power.length; i++) {
    if (power[i][0] === a.on && power[i][1] === a.off) {
      return i
    }
  }
  return -1
}

function storePresets (state) {
  let { preset1, preset2, preset3 } = state
  localStorage.setItem('presets', JSON.stringify([preset1, preset2, preset3]))
}

function loadPresets (id) {
  let obj = localStorage.getItem('presets')
  if (!obj) {
    return {
      fan0: 5,
      fan1: 5,
      tempWarmer: 20
    }
  }
  let presets = JSON.parse(obj)
  return presets[id]
}

let requestQueue = []

function updateFanSpeed (ip1, keyId, level) {
  let [on, off] = power[level]
  let id = Number(keyId[keyId.length - 1])
  // console.log(on, off, id)

  requestQueue = requestQueue.filter(e => e.type !== keyId)
  requestQueue.push({
    type: keyId,
    url: `http://${ip1}/fan?id=${id}&on=${on}&off=${off}`
  })
  serveRequests()
}

function updateWarmerTemp (ip1, value) {
  requestQueue = requestQueue.filter(e => e.type !== 'temp')
  requestQueue.push({
    type: 'temp',
    url: `http://${ip1}/warmer?temp=${value}`
  })
  serveRequests()
}

function serveRequests () {
  if (store.state.inProcess) {
    return
  }
  store.commit('setInPorcess', true)

  let routine = () => {
    let task = requestQueue.pop()
    console.log('=> ' + task.url)

    fetch(task.url).then(r => {
      if (!r.ok) {
        requestQueue.unshift(task)
      }
    }).catch(e => {
      requestQueue.unshift(task)
    }).finally(() => {
      if (requestQueue.length) {
        setTimeout(routine, 500)
      } else {
        store.commit('setInPorcess', false)
      }
    })
  }

  setTimeout(routine, 0)
}

export default store

let lastTimeUpdate = 0
function checkUpdate () {
  let now = Date.now()
  if (now - lastTimeUpdate > 60000) {
    lastTimeUpdate = now
    store.dispatch('read')
    store.commit('updateTime')
  }
}
let updateId = setInterval(checkUpdate, 1000)
checkUpdate()

function handleVisibilityChange () {
  clearInterval(updateId) // disable updates on page background
  if (document.webkitHidden) {
    lastTimeUpdate = 0 // force reload on next page open
  } else {
    updateId = setInterval(checkUpdate, 1000)
  }
}

document.addEventListener('webkitvisibilitychange', handleVisibilityChange, false)
