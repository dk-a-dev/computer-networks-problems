#include <stdio.h>

void stopAndWait(int total_frames)
{
    for (int i = 0; i < total_frames; i++)
    {
        printf("Transmitting frame %d\n", i + 1);
        char ack;
        printf("Acknowledge received for frame %d (Y/N): ", i + 1);
        scanf(" %c", &ack);
        
        if (ack == 'Y' || ack == 'y')
            printf("Frame %d acknowledged, moving to next frame.\n", i + 1);
        else
        {
            printf("Frame %d not acknowledged, resending.\n", i + 1);
            i--;
        }
    }
}
int main()
{
    int total_frames = 5;
    stopAndWait(total_frames);
    return 0;
}