<template>
  <div class="button-container">
    <div class="buttons">
      <div @touchstart="inc(pressId);" @touchend="pressId++"  class="button-up">
      </div>
      <div @touchstart="dec(pressId);" @touchend="pressId++" class="button-down">
      </div>
    </div>
    <div class="button-info">
      {{temp}}
    </div>
  </div>
</template>

<script>
export default {
  name: 'buttonud',
  props: ['sensor', 'max'],
  data () {
    return {
      pressId: 0
    }
  },
  computed: {
    temp () {
      return this.$store.state[this.sensor]
    }
  },
  methods: {
    inc: function (curId) {
      if (curId !== this.pressId) {
        return
      }
      if (this.temp < this.max) {
        this.$store.commit('warmerInc') // this.temp++
      }
      setTimeout(this.inc, 200, curId)
    },
    dec: function (curId) {
      if (curId !== this.pressId) {
        return
      }
      if (this.temp > 0) {
        this.$store.commit('warmerDec') // this.temp--
      }
      setTimeout(this.dec, 200, curId)
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>

.buttons {
  position: absolute;
}

.button-container {
  width: 110px;
  height: 180px;
  border: 1px solid black;
  margin: auto 6px;
}

.button-info {
  position: absolute;
  /*height: 140px;*/
  width: 110px;
  vertical-align: middle;
  line-height: 180px;
  font-size: 80px;
  font-family: sans-serif;
  z-index: -1;
  padding: auto 4px;
}

.button-up, .button-down {
  height: 90px;
  width: 110px;
  z-index: 100;
}

.test {
  color: #35495E;
}
</style>
