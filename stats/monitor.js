// influxdb exporter

const request = require('request-promise-native');

const conv = (raw) => {
  return JSON.parse(raw.replace(/-\d\.-\d/g, (a) => {
    return a.replace(/-/g, '');
  }));
}

const deviceMap = {
  fan: o => [{ id: o.id, val: (100 * o.on / (o.on + o.off)) }],
  relay: o => [{ id: o.id, val: o.on }],
  t_sensor: o => [{ id: o.id, val: o.t }],
  warmer: (o) => [
    { id: 'current_t', val: o.current_t },
    { id: 'target_t', val: o.target_t },
    { id: 'level', val: o.level },
    { id: 'ssr1', val: o.ssr1 },
    { id: 'ssr2', val: o.ssr2 },
  ],
  t_sensor22: o => [
    { id: 't', val: o.t },
    { id: 'h', val: o.h }
  ],
  barometr: o => [
    { id: 't', val: o.t },
    { id: 'p', val: o.p },
  ],
};

//[ { type: 't_sensor22', t: 0.9, h: 95.5, pin: 1 },
//  { type: 'barometr', p: 736.2, t: 3.5 } ]

//[ { type: 'fan', id: 0, on: 120, off: 80, pin: 4 },
//  { type: 'fan', id: 1, on: 80, off: 80, pin: 5 },
//  { type: 'relay', id: 0, on: false, pin: 6 },
//  { type: 'relay', id: 1, on: false, pin: 7 },
//  { type: 't_sensor', id: 0, t: 21.25, pin: 0 },
//  { type: 't_sensor', id: 1, t: 28.93, pin: 1 },
//  { type: 't_sensor', id: 2, t: 26.25, pin: 2 },
//  { type: 't_sensor', id: 3, t: 7.5, pin: 3 },
//  { type: 'warmer',
//    current_t: 21.25,
//    target_t: 22,
//    level: 1,
//    pin: 8,
//    ssr1: 28.93,
//    ssr2: 26.25 } ] 
const addResult = (res, data) => {
  let metrics = [];
  for (let d of data) {
    const getMetrics = deviceMap[d.type];
    const deviceMetrics = getMetrics(d);
    for (let m of deviceMetrics) {
      const r = `vent_${d.type},device=${m.id} value=${m.val}`;
      metrics.push(r);
    }
  }
  return res.concat(metrics);
}

const main = async () => {
  try {
    let res = [];
    const v1 = await request('http://192.168.0.177');
    const d1 = conv(v1);
    res = addResult(res, d1);
    const v2 = await request('http://192.168.0.209');
    const d2 = conv(v2);
    res = addResult(res, d2);
    res = res.join('\n');
    // console.log(res);

    const opts = {
      headers: {
        'content-type': 'application/x-www-form-urlencoded',
      },
      body: res,
    };
    const result = await request.post('http://localhost:8086/write?db=mydb', opts);
    console.log('result:', result);
  } catch (e) {
    console.log(e)
  }
}

// main();
setInterval(main, 5 * 60 * 1000);