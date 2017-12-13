<template>
  <div class="hello">
    <buttonud max=10></buttonud>
    <infobox></infobox>
    <buttonud></buttonud>
  </div>
</template>

<script>
import Button from '@/components/buttonud'
import InfoBox from '@/components/infobox'

export default {
  name: 'hello',
  data () {
    return {
      msg: 'Welcome to Your Vue.js PWA'
    }
  },
  components: {
    'buttonud': Button,
    'infobox': InfoBox
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>
.hello {
  display: flex;
  flex-direction: row;
}

h1, h2 {
  font-weight: normal;
}

ul {
  list-style-type: none;
  padding: 0;
}

li {
  display: inline-block;
  margin: 0 10px;
}

a {
  color: #35495E;
}
</style>
