<template>
  <div class="button-container">
    <div class="buttons">
      <div  @touchstart="inc(pressId);" @touchend="pressId++"  class="button-up">
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
  data () {
    return {
      temp: 20,
      pressId: 0
    }
  },
  props: [{
    max: {
      type: Number,
      default: 30
    }
  }],
  methods: {
    inc: function (curId) {
      console.log(this.max)
      if (curId !== this.pressId) {
        return
      }
      if (this.temp < this.max) {
        this.temp++
      }
      setTimeout(this.inc, 250, curId)
    },
    dec: function (curId) {
      if (curId !== this.pressId) {
        return
      }
      if (this.temp > 0) {
        this.temp--
      }
      setTimeout(this.dec, 250, curId)
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
  width: 140px;
  height: 140px;
  border: 1px solid black;
}

.button-info {
  position: absolute;
  height: 140px;
  width: 140px;
  vertical-align: middle;
  line-height: 140px;
  font-size: 80px;
  z-index: -1;
}

.button-up, .button-down {
  height: 70px;
  width: 140px;
  z-index: 100;
}

.test {
  color: #35495E;
}
</style>
