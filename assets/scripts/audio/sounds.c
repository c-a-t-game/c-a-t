#depends "scripts/audio/driver.c"

AudioSource* sound_jump() -> audio.create(lambda synth_jump(Synthesizer* synth): bool -> synth
    .saw(0.0625, note("C5")).slide(note("C#5")).volume_slide(0, 0.4)
    .saw(0.1875, note("C#5")).slide(note("E5")).volume_slide(0.4, 0)
    //.saw(0.25, note("E5")).volume_slide(1, 0)
.finish());

AudioSource* sound_get_coin() -> audio.create(lambda synth_get_coin(Synthesizer* synth): bool -> synth
    .square(0.1, note("D6")).volume_slide(0.5, 0)
.finish());

AudioSource* sound_get_heart() -> audio.create(lambda synth_get_heart(Synthesizer* synth): bool {
    for (int i = 0; i < 12; i++) {
        synth.square(i == 11 ? 0.2 : 0.1, note("C5") + i)
            .slide(note("F#5") + i)
            .volume_slide(0.25, i == 11 ? 0 : 0.20);
    }
    return synth.finish();
});

AudioSource* sound_get_hurt() -> audio.create(lambda synth_get_hurt(Synthesizer* synth): bool -> synth
    .noise(0.25, note("C9")).volume_slide(1, 0)
.finish());

AudioSource* sound_explosion() -> audio.create(lambda synth_explosion(Synthesizer* synth): bool -> synth
    .noise(0.75, note("C8")).volume_slide(1, 0)
.finish());

AudioSource* sound_stomp() -> audio.create(lambda synth_stomp(Synthesizer* synth): bool -> synth
    .noise(0.05, note("C9")).slide(note("C4")).volume_slide(0.75, 0)
.finish());

AudioSource* sound_kick() -> audio.create(lambda synth_kick(Synthesizer* synth): bool -> synth
    .noise(0.05, note("C8")).slide(note("C3")).volume_slide(0.75, 0)
.finish());

AudioSource* sound_break() -> audio.create(lambda synth_break(Synthesizer* synth): bool -> synth
    .noise(0.2, note("C8")).slide(note("C3")).volume_slide(0.75, 0)
.finish());

AudioSource* sound_transition() -> audio.create(lambda synth_transition(Synthesizer* synth): bool -> synth
    .noise(0.5, note("C7")).volume_slide(0, 0.125)
    .noise(0.5, note("C7")).volume_slide(0.125, 0)
.finish());

AudioSource* sound_step1() -> audio.create(lambda synth_step1(Synthesizer* synth): bool -> synth
    .noise(0.05, note("C7")).volume_slide(0.125, 0)
.finish());

AudioSource* sound_step2() -> audio.create(lambda synth_step2(Synthesizer* synth): bool -> synth
    .noise(0.05, note("F#6")).volume_slide(0.125, 0)
.finish());
