'use strict'

const g_update_period_min = 10; // ms
let g_update_period = 0; // ms

const audio_blip_no_answer = new Audio('sound/no_answer_peep.mp3');

function update() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/',
        data: { content: 'data' },
        success: (data) => {
            for (const key in data) {
                switch (key) {
                    case 'update_period':
                        //console.log(key + ' : ' + data[key]);
                        g_update_period = data[key];
                        if (g_update_period < g_update_period_min)
                            g_update_period = g_update_period_min;
                        break;
                    case 'datetime':
                    case 'uptime':
                        //console.log(key + ' : ' + data[key]);
                        SetElemText(key, data[key]);
                        break;
                    case 'cpu':
                        {
                            SetElemText('cpu_loadavg', data[key]['cpu_loadavg']);
                            SetElemText('cpu_thermal_text', data[key]['cpu_thermal']['text']);
                            SetElemBgColour('cpu_thermal_color', data[key]['cpu_thermal']['color']);
                        }
                        break;
                    case 'gnss':
                        {
                            for (const key2 in data[key]) {
                                //console.log(key2 + ' : ' + data[key][key2]);
                                switch (key2) {
                                    case 'status':
                                        SetElemText('status_text', data[key][key2]['text']);
                                        SetElemBgColour('status_color', data[key][key2]['color']);
                                        break;
                                    case 'mode':
                                        SetElemText('mode_text', data[key][key2]);
                                        break;
                                    case 'utc':
                                        SetElemText('utc_text', data[key][key2]);
                                        break;
                                    case 'location':
                                        SetElemText('location_text', data[key][key2]);
                                        break;
                                }
                            }
                        }
                    default:
                        //console.log(key);
                        break;
                }
            }

            if (g_update_period > 0)
                setTimeout(() => { update() }, g_update_period);
        },
        error: () => {
            if (g_update_period > 0)
                setTimeout(() => {
                    try {
                        // Uncaught(in promise) DOMException: play() failed because the user didn't interact with the document first.
                        audio_blip_no_answer.play();
                    } catch { }
                    update();
                }, g_update_period > 1000 ? g_update_period : 1000); // It's not frequently that once a second.
        },
    });
}

function SetElemText(a_key, a_data) {
    const elem = document.getElementById(a_key);
    if (elem != null && elem.innerText != a_data)
        elem.innerText = a_data;
}

function SetElemBgColour(a_key, a_data) {
    const elem = document.getElementById(a_key);
    if (elem != null)
        elem.setAttribute('bgcolor', a_data);
}