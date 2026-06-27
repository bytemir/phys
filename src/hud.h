typedef struct {
    float objectDimension;
    float objectMass;
    
    bool isGravity;
    bool isDrag;

    bool ballMenuEnabled;
    
} HudProperties;

void RenderHud(void);
HudProperties *GetHudProperties(void);