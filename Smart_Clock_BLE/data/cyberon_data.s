.pushsection command_data, "ax", %progbits
#.incbin "data/Trigger_and_command_pack_withTxt.bin"
.incbin "data/Test_pack_WithTxt.bin"
.popsection

.pushsection license_data, "ax", %progbits
#.incbin "data/F32F264A0E8B05EA1720686282_License.bin"
#.incbin "data/F52F2A1F028CAFE41718160741_License.bin"
.incbin "data/F52F2A1F028CAFE41724215169_License.bin"
.popsection
