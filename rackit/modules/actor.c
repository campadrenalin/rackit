typedef void(*ActorCallback)(void*, int);
typedef struct {
    void *data;
    ActorCallback callback;
} Actor;

int num_actors;
Actor actors[200];

void Actor_append(void *data, ActorCallback callback) {
    Actor a = { data, callback };
    actors[num_actors++] = a;
}
