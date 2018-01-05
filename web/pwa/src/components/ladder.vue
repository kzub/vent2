<template>
  <div class="ladder-container">
    <div class="ladders">
      <div @touchstart="inc(pressId);" @touchend="pressId++" class="ladder-up" >
      </div>
      <div @touchstart="dec(pressId);" @touchend="pressId++" class="ladder-down">
      </div>
    </div>
    <div class="ladder-info">
      <div :class="style(10)"></div>
      <div :class="style(9)"></div>
      <div :class="style(8)"></div>
      <div :class="style(7)"></div>
      <div :class="style(6)"></div>
      <div :class="style(5)"></div>
      <div :class="style(4)"></div>
      <div :class="style(3)"></div>
      <div :class="style(2)"></div>
      <div :class="style(1)"></div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ladderud',
  props: ['sensor'],
  data () {
    return {
      pressId: 0,
      max: 10
    }
  },
  computed: {
    value () {
      return this.$store.state[this.sensor]
    },
    blink () {
      let isOn = this.$store.state.presetBlink
      if (isOn) {
        setTimeout(() => {
          this.$store.commit('presetBlinkOff')
        }, 200)
      }
      return isOn
    }
  },
  methods: {
    inc: function (curId) {
      if (curId !== this.pressId) {
        return
      }
      if (this.value < this.max) {
        this.$store.commit('fanInc', this.sensor) // this.value++
      }
      setTimeout(this.inc, 200, curId)
    },
    dec: function (curId) {
      if (curId !== this.pressId) {
        return
      }
      if (this.value > 0) {
        this.$store.commit('fanDec', this.sensor) // this.value--
      }
      setTimeout(this.dec, 200, curId)
    },
    style: function (n) {
      if (this.blink) {
        return 'line line-preset'
      }
      if (n <= this.value) {
        return 'line line-on'
      }
      return 'line'
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>
.ladder-container {
  width: 110px;
  height: 180px;
  border: 1px solid gray;
  margin: auto 4px;
}

.ladder-info {
  position: absolute;
  vertical-align: middle;
  line-height: 110;
  font-size: 80px;
  z-index: -1;
  padding: 3px 2px;
}

.line {
  width: 104px;
  height: 14px;
  margin: 3px auto;
}

.line-on {
  background-color: #326596;
}

.line-preset {
  background-color: orange;
}

.ladders {
  position: absolute;
}

.ladder-up, .ladder-down {
  height: 90px;
  width: 110px;
  z-index: 100;
}

.test {
  color: #35495E;
}
</style>
