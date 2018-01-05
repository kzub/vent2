<template>
  <div @touchstart="press()" @touchend="press(true)" class="preset-container" :style="{ width: w, height: h, 'line-height': h }">
    <div class="preset-info" :style="{ 'font-size': fs }">
      {{name}}
    </div>
  </div>
</template>

<script>
export default {
  name: 'preset',
  props: ['name', 'dataKey', 'w', 'h', 'fs'],
  data () {
    return {
      timerId: 0
    }
  },
  methods: {
    press: function (end) {
      if (end) {
        if (this.timerId) {
          clearTimeout(this.timerId)
          this.timerId = undefined
          this.$store.commit('onPresetKey', this.dataKey)
        }
        return
      }

      if (this.timerId) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.timerId = undefined
        this.$store.commit('toPreset', this.dataKey)
        this.$store.commit('presetBlinkOn')
        setTimeout(() => {
          this.$store.commit('presetBlinkOff')
        }, 200)
      }, 1500)
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>

.buttons {
  position: absolute;
}

.preset-container {
  width: 80px;
  height: 50px;
  border: 1px solid grey;
  margin: auto 4px;
  font-family: sans-serif;
  background: #DDDDDD;
  display: flex;
  align-items: center;
  flex-direction: column;
  -moz-user-select: none;
  -webkit-user-select: none;
}

.preset-info {
  z-index: +1;
  font-size: 40px;
}
</style>
