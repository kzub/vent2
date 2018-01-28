<template>
  <div class="hello">
    <div class="row infoline">
      <ladder sensor="fan0"></ladder>
      <buttonud sensor="tempWarmer" max="30"></buttonud>
      <ladder sensor="fan1"></ladder>
    </div>
    <div class="row infoline">
      <infobox w="80px" h="44px" sensor="tempIn"></infobox>
      <infobox w="80px" h="44px" sensor="ssr1"></infobox>
      <infobox w="80px" h="44px" sensor="ssr2"></infobox>
      <infobox w="80px" h="44px" sensor="tempOut"></infobox>
    </div>
    <div class="row infoline">
      <infobox w="105px" h="60px" fs="30px" sensor="outT"></infobox>
      <infobox w="130px" h="60px" fs="30px" sensor="outP"></infobox>
      <infobox w="95px"  h="60px" fs="30px" sensor="outH"></infobox>
    </div>
    <div class="row infoline">
      <infobox w="350px" h="70px" fs="50px" sensor="date"></infobox>
    </div>
    <div class="row infoline">
      <infobox w="350px" h="80px" fs="70px" sensor="time"></infobox>
    </div>
    <div class="row infoline">
      <preset dataKey="preset1" name="L"></preset>
      <preset dataKey="preset2" name="N"></preset>
      <preset dataKey="preset3" name="H"></preset>
    </div>
  </div>
</template>

<script>
import Button from '@/components/buttonud'
import Ladder from '@/components/ladder'
import InfoBox from '@/components/infobox'
import Presets from '@/components/preset'

export default {
  name: 'hello',
  components: {
    'buttonud': Button,
    'ladder': Ladder,
    'infobox': InfoBox,
    'preset': Presets
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>
.infoline {
  margin: 5px auto;
}
.row {
  display: flex;
  flex-direction: row;
}

.hello {
  display: flex;
  flex-direction: column;
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
