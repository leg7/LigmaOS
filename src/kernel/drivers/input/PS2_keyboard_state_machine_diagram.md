## PS2 scan code to keycode state machine diagram

```{.mermaid format=pdf}
stateDiagram-v2
    classDef final fill:#318CE7

    INITIAL --> RELEASED : Released
    INITIAL --> EXTENDED : Extended
    INITIAL --> KEY_PRESSED : Scancode
    INITIAL --> PAUSE_PRESSED : Pause

    RELEASED --> KEY_RELEASED : Scancode

    KEY_RELEASED --> INITIAL
    KEY_PRESSED -->  INITIAL

    PAUSE_PRESSED --> KEY_PRESSED

    EXTENDED --> EXTENDED_RELEASED : Released
    EXTENDED --> KEY_PRESSED : Scancode_extended
    EXTENDED --> EXTENDED_PRINT_1 : Print_screen_1

    EXTENDED_PRINT_1 --> EXTENDED_PRINT_2 : Extended
    EXTENDED_PRINT_2 --> KEY_PRESSED : Print_screen_2

    EXTENDED_RELEASED --> KEY_RELEASED : Released
    EXTENDED_RELEASED --> EXTENDED_RELEASED_PRINT_1 : Print_screen_1
    EXTENDED_RELEASED_PRINT_1 --> EXTENDED_RELEASED_PRINT_2 : Extend
    EXTENDED_RELEASED_PRINT_2 --> EXTENDED_RELEASED_PRINT_3 : Released
    EXTENDED_RELEASED_PRINT_3 --> KEY_RELEASED : Print_screen_2

    class KEY_PRESSED, KEY_RELEASED final
```

Note: Unlabeled transitions occur in control flow

