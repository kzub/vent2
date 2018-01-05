<template>
  <div class="info-container" :style="{ width: w, height: h, 'line-height': h }">
    <div class="info-info" :style="{ 'font-size': fs }">
      {{temp}}
    </div>
  </div>
</template>

<script>
export default {
  name: 'infobox',
  props: ['sensor', 'w', 'h', 'fs'],
  computed: {
    temp () {
      return this.$store.state[this.sensor]
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>

.buttons {
  position: absolute;
}

.info-container {
  width: 90px;
  height: 60px;
  border: 1px solid grey;
  margin: auto 4px;
  font-family: sans-serif;
  background: lightgrey;
  display: flex;
  align-items: center;
  flex-direction: column;
}

.info-info {
  z-index: +1;
  font-size: 30px;
}
</style>
