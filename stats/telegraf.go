package main

import (
	"encoding/json"
	"fmt"
	_ "net"
)

func main() {
	var jsonBlob = []byte(`[{"type": "fan", "id":0, "on":100, "off":100, "pin":3},{"type": "fan", "id":1, "on":0, "off":0, "pin":4} ,
{"type": "relay", "id":0, "on":false, "pin":5},{"type": "relay", "id":1, "on":false, "pin":6},{"type": "relay", "id":2, "on":false, "pin":7},{"type": "relay", "id":3, "on":false, "pin":8} ,
{"type":"t_sensor", "id":0, "t": 17.50, "pin":0},{"type":"t_sensor", "id":1, "t": 22.18, "pin":1},{"type":"t_sensor", "id":2, "t": 11.81, "pin":2} ,
{"type": "warmer", "current_t": 17.50, "target_t":10, "level":3, "pin":9}
]`)
	type Device struct {
		Type     string
		Id       int
		T        float32
		Level    int
		Target_t float32
		On       interface{}
		Off      int
		P        float32
		H        float32
	}

	var devices []Device
	err := json.Unmarshal(jsonBlob, &devices)
	if err != nil {
		fmt.Println("error:", err)
	}

	tags := make(map[string]string)
	_ = tags
	addStat := func(fields map[string]interface{}) {
		for key, value := range fields {
			fmt.Println(key, value)

		}
		// acc.AddFields("vent_stats", fields, tags)
	}

	for _, device := range devices {
		if device.Type == "t_sensor" {
			k := fmt.Sprintf("%s_%d", device.Type, device.Id)
			field := make(map[string]interface{})
			field[k] = device.T
			addStat(field)
		} else if device.Type == "warmer" {
			field := make(map[string]interface{})
			field["warmer_level"] = device.Level
			field["warmer_target_t"] = device.Target_t
			addStat(field)
		} else if device.Type == "fan" {
			k := fmt.Sprintf("%s_%d", device.Type, device.Id)
			field := make(map[string]interface{})
			if device.On == 0 {
				field[k] = 0
			} else if device.Off == 0 {
				field[k] = 100
			} else {
				on := int(device.On.(float64))
				off := device.Off
				field[k] = 100 * on / (on + off)
			}
			addStat(field)
		} else if device.Type == "t_sensor22" {
			field := make(map[string]interface{})
			field["dht22_t"] = device.T
			field["dht22_humidity"] = device.H
			addStat(field)
		} else if device.Type == "barometr" {
			field := make(map[string]interface{})
			field["barometr_t"] = device.T
			field["barometr_p"] = device.P
			addStat(field)
		}
	}
}
