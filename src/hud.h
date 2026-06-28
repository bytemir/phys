typedef struct {
    float objectDimension;
    float objectMass;

    float springStiffness;
    float springDamping;
    
    bool isGravity;
    bool isDrag;

    bool ballMenuEnabled;
    bool springMenuEnabled;
    bool rampMenuEnabled;
    
} HudProperties;

void RenderHud(void);
HudProperties *GetHudProperties(void);