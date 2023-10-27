'use strict';

const np_fs = require('fs');

let g_ReadPipe = {};

exports.StartPipeReading = function (a_config) {
    ReadPipe(a_config.pipe['card_mifare']);
}

exports.GetPage = function () {
    return '<tr><td></td><td>Card Type:</td><td id="card_type"></td></tr>' +
        '<tr><td id="card_status" bgcolor="white"></td><td>Card UID:</td><td id="card_uid"></td></tr>';
}

exports.GetPageData = function (a_config) {
    let data = {};
    data.card_type = GetCardType(g_ReadPipe);
    data.card_status = 'yellow';
    data.card_data = g_ReadPipe;
    return data;
}

function ReadPipe(a_pipeName) {
    try {
        const pipeRead = np_fs.createReadStream(a_pipeName + '_o', 'utf-8');
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
    setTimeout(() => { ReadPipe(a_pipeName) }, 100); // Nyquist sampling theorem 1/2F (?)
}

function GetCardType(a_card) {
    // http://www.nxp.com/documents/application_note/AN10833.pdf 
    // 3.2 Coding of Select Acknowledge (SAK)
    // ignore 8-bit (iso14443 starts with LSBit = bit 1)
    // fixes wrong type for manufacturer Infineon (http://nfc-tools.org/index.php?title=ISO14443A)
    if (a_card === undefined || typeof (a_card.uid) != 'string' || a_card.uid.length == 0)
        return '';
    let sak = Number(a_card.sak);
    sak &= 0x7F;
    switch (sak) {
        case 0x04: return 'UID not complete';  // UID not complete
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