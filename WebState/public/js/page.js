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
                    //console.log(key + ' : ' + data[key]);
                    case 'update_period':
                        //console.log(key + ' : ' + data[key]);
                        g_update_period = data[key];
                        if (g_update_period < g_update_period_min)
                            g_update_period = g_update_period_min;
                        break;
                    case 'host':
                        SetElemText('host_utc', data[key]['utc']);
                        SetElemText('host_uptime', data[key]['uptime']);
                        SetElemBgColor('host_color', data[key]['color'] ?? '');
                        break;
                    case 'cpu':
                        {
                            SetElemText('cpu_loadavg', data[key]['cpu_loadavg']);
                            SetElemText('cpu_thermal_text', data[key]['cpu_thermal']['text']);
                            SetElemBgColor('cpu_thermal_color', data[key]['cpu_thermal']['color']);
                        }
                        break;
                    case 'gnss':
                        {
                            SetElemText('gnss_status_text', data[key]['status']?.['text']);
                            SetElemClass('gnss_status_text', data[key]['status']?.['class'] ?? '')
                            SetElemBgColor('gnss_status_color', data[key]['status']?.['color']);
                            SetElemText('gnss_utc_file_text', data[key]['utc_file'] ?? '');
                            SetElemText('gnss_mode_text', data[key]['mode'] ?? '');
                            SetElemText('gnss_utc_text', data[key]['utc'] ?? '');
                            SetElemText('gnss_sats_gps', data[key]['sats']?.['gps']?.['text'] ?? '');
                            SetElemClass('gnss_sats_gps', data[key]['sats']?.['gps']?.['class'] ?? '')
                            SetElemText('gnss_sats_glo', data[key]['sats']?.['glo']?.['text'] ?? '');
                            SetElemClass('gnss_sats_glo', data[key]['sats']?.['glo']?.['class'] ?? '')
                            SetElemText('gnss_location_text', data[key]['location'] ?? '');
                            SetElemText('gnss_speed_text', data[key]['speed'] ?? '');
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

function SetElemBgColor(a_key, a_data) {
    document.getElementById(a_key)?.setAttribute('bgcolor', a_data);
}

function SetElemClass(a_key, a_class) {
    document.getElementById(a_key)?.setAttribute('class', a_class);
}
