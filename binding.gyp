{
  "targets": [
    {
      "target_name": "settings",
      "type": "none",
      "all_dependent_settings": {
        "include_dirs+": [
          "<!@(node -p \"require('node-addon-api').include\")",
          "native/lib",
        ],
        "cflags!": [
          "-fno-exceptions"
        ],
        "cflags_cc!": [
          "-fno-exceptions"
        ],
        "defines+": [
          "NAPI_DISABLE_CPP_EXCEPTIONS"
        ],
        "conditions": [
          [
            "OS==\"win\"",
            {
              "defines+": [
                "WIN32_LEAN_AND_MEAN"
              ],
            }
          ],
          [
            "OS==\"mac\"",
            {
            }
          ]
        ],
      }
    },
    {
      "target_name": "native-application-scanner",
      "dependencies": [
        "settings"
      ],
      "sources": [
        "native/lib/application-scanner/application-scanner.cpp"
      ],
      "conditions": [
        [
          "OS==\"win\"",
          {
            "sources+": [
              "native/lib/utilities/helpers.windows.cpp",
              "native/lib/application-scanner/application-scanner.windows.cpp"
            ],
            "link_settings": {
              "libraries+": [
                "dwmapi.lib",
              ]
            }
          }
        ],
        [
          "OS==\"mac\"",
          {
            "sources+": [
              "native/lib/applicaiton-scanner/application-scanner.mac.cpp"
            ]
          }
        ]
      ]
    },
    {
      "target_name": "native-application-status",
      "dependencies": [
        "settings"
      ],
      "sources": [
        "native/lib/application-status/application-status.cpp"
      ],
      "conditions": [
        [
          "OS==\"win\"",
          {
            "sources+": [
              "native/lib/utilities/helpers.windows.cpp",
              "native/lib/application-status/application-status.windows.cpp"
            ],
            "link_settings": {
              "libraries+": [
                "dwmapi.lib",
                "user32.lib" 
              ]
            }
          }
        ],
      ]
    }
  ]
}