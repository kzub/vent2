<template>
  <div class="info-container">
    <div class="info-info">
      {{temp}}
    </div>
  </div>
</template>

<script>
export default {
  name: 'infobox',
  data () {
    return {
      temp: 20
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
  width: 140px;
  height: 140px;
  border: 1px solid black;
}

.info-info {
  position: absolute;
  height: 140px;
  width: 140px;
  vertical-align: middle;
  line-height: 140px;
  z-index: -1;
}

.info-up, .info-down {
  height: 70px;
  width: 140px;
  z-index: 100;
}

.test {
  color: #35495E;
}
</style>
