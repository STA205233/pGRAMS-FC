HSQuickLook.main.schema =
    [
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "Status",
	      "period": 1,
	      "section": "Header",
	      "contents": {
          "Start_Code": {"type": "int", "format": "0x%04X"},
          "Telemetry_Type": {"type": "int"},
          "Time": {"type": "int"},
          "Time_us": {"type": "int"},
          "Telemetry_Index": {"type": "int"}
        }
      },
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "Status",
	      "period": 1,
	      "section": "Status",
	      "contents": {
          "Trigger_Mode": {"type": "int"},
          "Trigger_Device": {"type": "int"},
          "Trigger_Channel": {"type": "int"},
          "Trigger_Level": {"type": "double"},
          "Trigger_Position": {"type": "double"},
          "Channel_Mask": {"type": "int"},
          "ADC_Offset_1": {"type": "double"},
          "ADC_Offset_2": {"type": "double"},
          "ADC_Offset_3": {"type": "double"},
          "ADC_Offset_4": {"type": "double"},
          "ADC_Range_1": {"type": "double"},
          "ADC_Range_2": {"type": "double"},
          "ADC_Range_3": {"type": "double"},
          "ADC_Range_4": {"type": "double"},
          "SD_Capacity": {"type": "int"}
        }
      },
      {
	      "collection": "grams",
	      "directory": "Telemetry",
	      "document": "Status",
	      "period": 1,
	      "section": "Footer",
	      "contents": {
          "CRC": {"type": "int"},
          "Stop_Code": {"type": "int", "format": "0x%04X"}
        }
      }      
    ];