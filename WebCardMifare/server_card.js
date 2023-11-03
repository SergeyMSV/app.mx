'use strict';

const np_fs = require('fs');

let g_PipeName;
let g_ReadPipe = {};

exports.StartPipeReading = function (a_config) {
    const startRead = g_PipeName === undefined;
    g_PipeName = a_config.pipe['card_mifare'];
    if (startRead)
        ReadPipe();
}

exports.GetPage = function () {
    return '<tr><td id="card_status" bgcolor="white"></td><td width="100">Card:</td><td id="card_type"></td></tr>' +
        '<tr><td></td><td id="card_content" colspan="2">' +
        '<table>' +
        '<tr><td>UID:</td><td id="card_uid" class="hex_box"></td></tr>' +
        '<tr><td>Payload:<br><br>' +
        '<button id="card_keep" onkeypress="card_keep();" onclick="card_keep();">Keep</button><br><br>' +
        '<button id="card_write" onkeypress="card_write();" onclick="card_write();">Write</button>' +
        '</td><td id="card_payload" class="hex_box"></td></tr>' +
        '</td></tr>' +
        '</table>' +
        '</td></td></tr>';
}

exports.GetPageData = function (a_config) {
    let data = {};
    data.card_type = GetCardType(g_ReadPipe);
    data.card_status = GetCardTypeColour(g_ReadPipe);
    data.card_data = g_ReadPipe;
    return data;
}

exports.CardWrite = function (a_config, a_data) {
    try {
        let Res = false;
        const pipeWrite = np_fs.createWriteStream(a_config.pipe['card_mifare'] + '_i', 'utf-8');
        try {
            pipeWrite.write(JSON.stringify(a_data));
            Res = true;
        }
        catch { }
        pipeWrite.close();
        return Res;
    }
    catch (err) {
        console.error(err);
    }
    return false;
}

function ReadPipe() {
    try {
        const pipeRead = np_fs.createReadStream(g_PipeName + '_o', 'utf-8');
        pipeRead.on('data', (chunk) => {
            try {
                g_ReadPipe = JSON.parse(chunk);
            }
            catch { }
            pipeRead.close();
        });
    }
    catch (err) {
        console.error(err);
    }
    setTimeout(() => { ReadPipe() }, 100); // Nyquist sampling theorem 1/2F (?)
}

function GetCardTypeNum(a_card) {
    // http://www.nxp.com/documents/application_note/AN10833.pdf 
    // 3.2 Coding of Select Acknowledge (SAK)
    // ignore 8-bit (iso14443 starts with LSBit = bit 1)
    // fixes wrong type for manufacturer Infineon (http://nfc-tools.org/index.php?title=ISO14443A)
    if (a_card === undefined || typeof (a_card.uid) != 'string' || a_card.uid.length == 0)
        return -1;
    let sak = Number(a_card.sak);
    sak &= 0x7F;
    return sak;
}

function GetCardType(a_card) {
    const cardType = GetCardTypeNum(a_card);
    switch (cardType) {
        case -1: return '';
        case 0x04: return 'UID not complete';
        case 0x09: return 'MIFARE MINI';
        case 0x08: return 'MIFARE 1K';
        case 0x18: return 'MIFARE 4K';
        case 0x00: return 'MIFARE UL';
        case 0x10:
        case 0x11: return 'MIFARE PLUS';
        case 0x01: return 'TNP3XXX';
        case 0x20: return 'ISO 14443 4';
        case 0x40: return 'ISO 18092';
        default: return 'UNKNOWN';
    }
}

function GetCardTypeColour(a_card) {
    const cardType = GetCardTypeNum(a_card);
    switch (cardType) {
        case -1: return 'gray';
        case 0x04: return 'red'; // UID not complete
        case 0x09: // MIFARE MINI
        case 0x08: // MIFARE 1K
        case 0x18: return 'yellow'; // MIFARE 4K

        case 0x00: return 'green'; // MIFARE UL
        case 0x10:
        case 0x11: // MIFARE PLUS
        case 0x01: // TNP3XXX
        case 0x20: // ISO 14443 4
        case 0x40: return 'yellow'; // ISO 18092;
        default: return 'orange';
    }
}