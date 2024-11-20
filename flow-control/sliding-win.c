#include <stdio.h>
void slidingWindow(int total_frames, int window_size)
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
        char ack;
        printf("Acknowledge received (Y/N): ");
        scanf(" %c", &ack);
        if (ack == 'Y' || ack == 'y')
        {
            i += window_size;
        }
        else
        {
            printf("Resending the frames\n");
        }
    }
}
int main()
{
    int total_frames = 10;
    int window_size = 4;
    slidingWindow(total_frames, window_size);
    return 0;
}
