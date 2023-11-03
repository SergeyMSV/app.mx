'use strict'

const g_update_period_min = 10; // ms
let g_update_period = 0; // ms

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
                            for (const key2 in data[key]) {
                                //console.log(key2 + ' : ' + data[key][key2]);
                                switch (key2) {
                                    case 'cpu_loadavg':
                                    case 'cpu_thermal':
                                        SetElemText(key2, data[key][key2]);
                                        break;
                                    case 'cpu_thermal_status':
                                        SetElemBgColour(key2, data[key][key2]);
                                        break;
                                }
                            }
                        }
                        break;
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
                setTimeout(() => { window.location.reload() }, g_update_period);
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