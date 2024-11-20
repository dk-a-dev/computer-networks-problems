#include <stdio.h>

void goBackN(int total_frames, int window_size)
{
    int i = 0;
    while (i < total_frames)
    {
        printf("Transmitting frames: ");
        for (int j = i; j < i + window_size && j < total_frames; j++)
        {
            printf("%d ", j + 1);
        }
        printf("\n");
        int ack;
        printf("Enter the last frame number received correctly: ");
        scanf("%d", &ack);
        if (ack >= i + window_size)
        {
            i += window_size;
        }
        else
        {
            printf("Resending from frame %d\n", ack + 1);
            i = ack;
        }
    }
}
int main()
{
    
    int total_frames;
    scanf("%d",&total_frames);
    int window_size;
     scanf("%d",&window_size);
    goBackN(total_frames, window_size);
    return 0;
}