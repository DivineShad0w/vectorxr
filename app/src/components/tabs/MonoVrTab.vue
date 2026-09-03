<script setup lang="ts">
import { computed, ref } from 'vue'

import ModuleBindingPage from '../ModuleBindingPage.vue'
import ModuleBindingPanel from '../ModuleBindingPanel.vue'
import ProfileShell from '../ProfileShell.vue'
import type { MonoVrConfig, MonoVrModuleConfig, RegisteredApplication } from '../../lib/model'

const props = defineProps<{
  config: MonoVrModuleConfig
  applications: RegisteredApplication[]
}>()

const emit = defineEmits<{
  addProfile: []
  removeProfile: [index: number]
  syncProfileName: [index: number]
}>()

const defaults = computed(() => props.config.defaults)

let bindingTarget: MonoVrConfig | null = null
const showBindingEditor = ref(false)

function openBinding(index: number) {
  bindingTarget = props.config.profiles[index].settings
  showBindingEditor.value = true
}

function openDefaultBinding() {
  bindingTarget = defaults.value
  showBindingEditor.value = true
}
</script>

<template>
  <ModuleBindingPage
    v-if="showBindingEditor && bindingTarget"
    module-label="Mono VR"
    :binding="bindingTarget.toggleBinding"
    label="Toggle Binding"
    description="Press the assigned control to temporarily collapse stereo into mono. Press again to restore stereo."
    none-text="No binding assigned. Mono VR stays in its configured state until you assign a toggle control."
    default-activate-sound="sound/activate.wav"
    default-deactivate-sound="sound/deactivate.wav"
    @close="showBindingEditor = false"
    @update:binding="bindingTarget!.toggleBinding = $event"
  />

  <div v-else class="space-y-4">
    <article class="rounded-[1.25rem] border p-5 shadow-panel backdrop-blur surface-panel">
      <div class="mb-4">
        <h2 class="text-2xl font-semibold tracking-tight">Mono VR</h2>
        <p class="mt-2 max-w-3xl text-sm leading-6 text-muted">
          Renders the scene monoscopically. Soft mode mirrors the left eye onto the right inside the
          layer — a comfort fix, the application still renders both views. Primary mode makes the
          application itself render a single viewport (half the GPU pixel workload); the layer
          duplicates the frame for the compositor. Primary is fixed when the application starts —
          restart it after switching modes.
        </p>
      </div>

      <section>
        <div class="mb-3">
          <p class="eyebrow text-xs font-semibold uppercase tracking-[0.24em]">Default Profile</p>
          <p class="mt-1 text-sm text-muted">Applies to applications without a custom profile.</p>
        </div>

        <div class="rounded-[1rem] border p-4 surface-panel-soft">
          <div class="flex flex-wrap items-center justify-between gap-3">
            <div class="max-w-2xl">
              <h3 class="text-base font-semibold tracking-tight">Enable Mono VR</h3>
              <p class="mt-1 text-sm leading-6 text-muted">
                {{ config.enabled
                  ? 'Mono applies to applications without a custom profile.'
                  : 'Mono stays off unless an enabled custom profile turns it on.' }}
              </p>
            </div>
            <label class="pill-toggle inline-flex items-center gap-3 rounded-full px-4 py-2 text-sm font-medium">
              <input v-model="config.enabled" class="h-4 w-4 accent-depthxr-copper" type="checkbox" />
              Default {{ config.enabled ? 'On' : 'Off' }}
            </label>
          </div>
        </div>

        <div class="mt-4 space-y-3">
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block">
              <span class="mb-1.5 flex min-h-[2.5rem] items-start gap-1.5 text-sm font-medium">
                Mode
                <span
                  title="Soft: the layer mirrors the left eye onto the right (no GPU savings). Primary: the application renders one viewport and the layer duplicates it for the compositor (real GPU savings). Primary takes effect when the application starts."
                  class="cursor-help select-none text-xs text-muted"
                  >ⓘ</span
                >
              </span>
              <select
                v-model="defaults.mode"
                class="app-input w-full rounded-[0.75rem] px-4 py-2.5"
              >
                <option value="soft">Soft — layer mirrors the views</option>
                <option value="primary">Primary — application renders one viewport</option>
              </select>
            </label>
            <p v-if="defaults.mode === 'primary'" class="mt-2 text-xs text-muted">
              Changes to Primary take effect when an application starts; restart the app after
              switching modes.
            </p>
          </div>

          <ModuleBindingPanel
            heading="Toggle Binding"
            :binding="defaults.toggleBinding"
            hint="Press the assigned control to temporarily collapse stereo into mono. Press again to restore stereo."
            @edit="openDefaultBinding()"
          />

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="flex items-start gap-3">
              <input
                v-model="defaults.invertedToggle"
                class="mt-0.5 h-4 w-4 accent-depthxr-copper"
                type="checkbox"
              />
              <div>
                <span class="block text-sm font-medium">Inverted toggle</span>
                <span class="block text-sm leading-6 text-muted">
                  {{ defaults.invertedToggle
                    ? 'Binding enables Mono VR (off by default, press to activate).'
                    : 'Binding disables Mono VR (on by default, press to suspend).' }}
                </span>
              </div>
            </label>
          </div>
        </div>
      </section>
    </article>

    <section class="space-y-3">
      <div class="sticky top-0 z-20 flex flex-wrap items-center justify-between gap-3 rounded-[1rem] border px-4 py-3 shadow-panel backdrop-blur surface-panel-strong">
        <div>
          <h2 class="text-lg font-semibold tracking-tight">Custom Profiles</h2>
          <p class="text-sm text-muted">Per-application settings for Mono VR.</p>
        </div>
        <button
          class="button-accent rounded-[0.75rem] px-5 py-2.5 text-sm font-medium"
          type="button"
          @click="$emit('addProfile')"
        >
          Add Profile
        </button>
      </div>

      <div v-if="config.enabled" class="rounded-[0.9rem] border px-4 py-3 text-sm leading-6 chip-warning" style="border-color: var(--app-border)">
        <strong>Broad enablement is active.</strong> Mono VR applies to applications without a custom profile, including ones you have not validated. The safer setup is Default Off with this list used as an application allowlist.
      </div>

      <ProfileShell
        v-for="(profile, index) in config.profiles"
        :key="index"
        :index="index"
        :profile="profile"
        :applications="applications"
        module-label="Mono VR"
        @remove="$emit('removeProfile', index)"
        @sync-name="$emit('syncProfileName', index)"
      >
        <div class="space-y-3">
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block">
              <span class="mb-1.5 flex min-h-[2.5rem] items-start gap-1.5 text-sm font-medium">
                Mode
                <span
                  title="Soft: the layer mirrors the left eye onto the right (no GPU savings). Primary: the application renders one viewport and the layer duplicates it for the compositor (real GPU savings). Primary takes effect when the application starts."
                  class="cursor-help select-none text-xs text-muted"
                  >ⓘ</span
                >
              </span>
              <select
                v-model="profile.settings.mode"
                class="app-input w-full rounded-[0.75rem] px-4 py-2.5"
              >
                <option value="soft">Soft — layer mirrors the views</option>
                <option value="primary">Primary — application renders one viewport</option>
              </select>
            </label>
            <p v-if="profile.settings.mode === 'primary'" class="mt-2 text-xs text-muted">
              Takes effect when the application starts; restart the app after switching modes.
            </p>
          </div>

          <ModuleBindingPanel
            heading="Toggle Binding"
            :binding="profile.settings.toggleBinding"
            hint="Press the assigned control to temporarily collapse stereo into mono. Press again to restore stereo."
            @edit="openBinding(index)"
          />

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="flex items-start gap-3">
              <input
                v-model="profile.settings.invertedToggle"
                class="mt-0.5 h-4 w-4 accent-depthxr-copper"
                type="checkbox"
              />
              <div>
                <span class="block text-sm font-medium">Inverted toggle</span>
                <span class="block text-sm leading-6 text-muted">
                  {{ profile.settings.invertedToggle
                    ? 'Binding enables Mono VR (off by default, press to activate).'
                    : 'Binding disables Mono VR (on by default, press to suspend).' }}
                </span>
              </div>
            </label>
          </div>
        </div>
      </ProfileShell>

      <div
        v-if="config.profiles.length === 0"
        class="rounded-[1rem] border border-dashed px-6 py-7 text-center text-sm surface-panel-soft"
      >
        No custom profiles yet. Add one when an application needs Mono VR on while the default stays off (or vice versa).
      </div>
    </section>
  </div>
</template>
