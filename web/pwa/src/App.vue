<template>
  <div id="app">
    <header @touchstart="showSettings()">
      <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, minimum-scale=1, user-scalable=false">
      <!-- <span>Room Climate</span> -->
    </header>
    <main>
      <!-- <img src="./assets/logo.png" alt="Vue.js PWA"> -->
      <router-view></router-view>
      <!-- settings window -->
      <div :class="test">
        <div class="settings-content">
          <div>
            <label>IP1:</label> <input id="ip1" class="ip" type="text" :value="$store.state.ip1"><br>
          </div>
          <div>
            <label>IP2:</label> <input id="ip2" class="ip" type="text" :value="$store.state.ip2"><br>
          </div>
          <button @touchstart="hideSettings()" class="ip">OK</button><br>
        </div>
      </div>
    </main>
  </div>
</template>

<script>
let lastTouch = 0
export default {
  name: 'app',
  data () {
    return {
      test: 'settings-hide'
    }
  },
  methods: {
    showSettings: function () {
      let now = Date.now()
      if (now - lastTouch > 250) {
        lastTouch = now
        return
      }

      this.test = 'settings-show'
      console.log('settings->', this.test)
    },
    hideSettings: function () {
      this.test = 'settings-hide'
      let ip1 = document.getElementById('ip1').value
      let ip2 = document.getElementById('ip2').value
      console.log(ip1, ip2)
      this.$store.commit('setIPs', { ip1, ip2 })
    }
  }
}
</script>

<style>
body {
  margin: 0;
}

/* Modal Content/Box */
.settings-show {
  display: block;
  position: fixed; /* Stay in place */
  z-index: 10; /* Sit on top */
  left: 0;
  top: 0;
  width: 100%; /* Full width */
  height: 100%; /* Full height */
  overflow: auto; /* Enable scroll if needed */
  background-color: rgb(0,0,0); /* Fallback color */
  background-color: rgba(0,0,0,0.7); /* Black w/ opacity */
}

.settings-hide {
  display: none
}

.settings-content {
  background-color: #aaaaaa;
  margin: 15% auto; /* 15% from the top and centered */
  padding: 20px;
  border: 1px solid #888;
  width: 80%; /* Could be more or less, depending on screen size */
}

.ip {
  border: 1px solid #888;
  height: 40px;
  font-size: 20px;
  margin: 6px;
}

#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
}

main {
  text-align: center;
  margin-top: 10px;
}

header {
  margin: 0;
  height: 40px;
  padding: 0 16px 0 24px;
  background-color: #35495E;
  color: #ffffff;
}

header span {
  display: block;
  position: relative;
  font-size: 20px;
  line-height: 1;
  letter-spacing: .02em;
  font-weight: 400;
  box-sizing: border-box;
  padding-top: 16px;
}
</style>
