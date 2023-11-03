'use strict'

const g_update_period_min = 10; // ms
let g_update_period = 0; // ms

const audio_blip_no_answer = new Audio('sound/blip_no_answer.wav');

let g_cardActive;
let g_cardStored;

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
                    case 'card':
                        {
                            let CardValid = false;
                            for (const key2 in data[key]) {
                                //console.log(key2 + ' : ' + data[key][key2]);
                                switch (key2) {
                                    case 'card_type':
                                        SetElemText(key2, data[key][key2]);
                                        break;
                                    case 'card_status':
                                        CardValid = data[key][key2] === 'green';
                                        SetElemBgColour(key2, data[key][key2]);
                                        break;
                                    case 'card_data':
                                        g_cardActive = data[key][key2];
                                        UpdateButtons(CardValid);
                                        SetElemText('card_uid', ToHexStr(data[key][key2]['uid']));
                                        SetElemText('card_payload', ToHexStr(data[key][key2]['payload']));
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

function SetElemEnabled(a_key, a_data) {
    const elem = document.getElementById(a_key);
    if (elem != null)
        elem.disabled = !a_data;
}

function UpdateButtons(a_cardValid) {
    const buttonBoth = a_cardValid && g_cardActive !== undefined && g_cardActive.uid.length > 0;
    const buttonKeep = buttonBoth && (g_cardStored === undefined || g_cardActive.uid != g_cardStored.uid);
    const buttonWrite = buttonBoth && g_cardStored !== undefined && g_cardStored.uid != g_cardActive.uid;
    SetElemEnabled('card_keep', buttonKeep);
    SetElemEnabled('card_write', buttonWrite);
}

function ToHexStr(a_val) {
    let str = "";
    for (let i = 0; i < a_val.length; ++i) {
        if (i && i % 16 == 0) {
            str += '\n';
        }
        else if (i && i % 2 == 0) {
            str += ' ';
        }
        str += a_val[i].toUpperCase();
    }
    return str;
}

function card_keep() {
    if (g_cardActive !== undefined) {
        g_cardStored = g_cardActive;
        window.alert('Put an empty RFID Card onto the card reader/writer.\nThen button "Write" is unblocked.');
    }
}

function card_write() {
    if (g_cardStored === undefined)
        return;
    $.ajax({
        type: 'post',
        url: '/card_write',
        //dataType: 'application/json',
        data: g_cardStored,
        //data: JSON.stringify(g_cardStored),
        success: (data, status, xhr) => {
            window.alert('RFID Card has been written successfully.');
        },
        error: (xhr, status, error) => {
            window.alert('RFID Card has not been written. \n' + error);
            console.log(error);
        },
    });
}