<script setup lang="ts">
import { computed, ref } from 'vue'

import ModuleBindingPage from '../ModuleBindingPage.vue'
import ModuleBindingPanel from '../ModuleBindingPanel.vue'
import ProfileShell from '../ProfileShell.vue'
import type { HeadCursorConfig, HeadCursorModuleConfig, HeadCursorProfileConfig, InputBinding, RegisteredApplication } from '../../lib/model'

const props = defineProps<{
  config: HeadCursorModuleConfig
  applications: RegisteredApplication[]
}>()

const emit = defineEmits<{
  addProfile: []
  removeProfile: [index: number]
  syncProfileName: [index: number]
}>()

const defaults = computed(() => props.config.defaults)

let bindingTarget: HeadCursorConfig | null = null
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
    module-label="Head Cursor"
    :binding="bindingTarget.toggleBinding"
    label="Toggle Binding"
    description="Press the assigned control to temporarily disable Head Cursor. Press again to re-enable."
    none-text="No binding assigned. Head Cursor stays active unless you assign a toggle control."
    default-activate-sound="sound/activate.wav"
    default-deactivate-sound="sound/deactivate.wav"
    @close="showBindingEditor = false"
    @update:binding="bindingTarget!.toggleBinding = $event"
  />

  <div v-else class="space-y-4">
    <article class="rounded-[1.25rem] border p-5 shadow-panel backdrop-blur surface-panel">
      <div class="mb-4">
        <h2 class="text-2xl font-semibold tracking-tight">Head Cursor</h2>
        <p class="mt-2 max-w-3xl text-sm leading-6 text-muted">
          Control the mouse cursor with head movements. Useful for menu navigation and in-game UI interaction without reaching for a controller.
        </p>
      </div>

      <section>
        <div class="mb-3">
          <p class="eyebrow text-xs font-semibold uppercase tracking-[0.24em]">Default Profile</p>
          <p class="mt-1 text-sm text-muted">Applies to applications without a custom profile. Tune sensitivity and deadzone to your preference.</p>
        </div>

        <div class="rounded-[1rem] border p-4 surface-panel-soft">
          <div class="flex flex-wrap items-center justify-between gap-3">
            <div class="max-w-2xl">
              <h3 class="text-base font-semibold tracking-tight">Enable Head Cursor</h3>
              <p class="mt-1 text-sm leading-6 text-muted">
                {{ config.enabled
                  ? 'Head Cursor applies to applications without a custom profile.'
                  : 'Head Cursor stays off unless an enabled custom profile turns it on.' }}
              </p>
            </div>
            <label class="pill-toggle inline-flex items-center gap-3 rounded-full px-4 py-2 text-sm font-medium">
              <input v-model="config.enabled" class="h-4 w-4 accent-depthxr-copper" type="checkbox" />
              Default {{ config.enabled ? 'On' : 'Off' }}
            </label>
          </div>
        </div>

        <div class="mt-4 grid gap-4 md:grid-cols-2">
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Yaw sensitivity</label>
            <input
              v-model.number="defaults.yawSensitivity"
              type="range"
              min="0.1"
              max="5"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.yawSensitivity.toFixed(1) }}</p>
            <p class="text-xs text-muted">How quickly horizontal head movement translates to cursor speed.</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Pitch sensitivity</label>
            <input
              v-model.number="defaults.pitchSensitivity"
              type="range"
              min="0.1"
              max="5"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.pitchSensitivity.toFixed(1) }}</p>
            <p class="text-xs text-muted">How quickly vertical head movement translates to cursor speed.</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Yaw multiplier</label>
            <input
              v-model.number="defaults.yawMultiplier"
              type="range"
              min="0.5"
              max="10"
              step="0.5"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.yawMultiplier.toFixed(1) }}</p>
            <p class="text-xs text-muted">Scale factor for horizontal cursor displacement.</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Pitch multiplier</label>
            <input
              v-model.number="defaults.pitchMultiplier"
              type="range"
              min="0.5"
              max="10"
              step="0.5"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.pitchMultiplier.toFixed(1) }}</p>
            <p class="text-xs text-muted">Scale factor for vertical cursor displacement.</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Deadzone (degrees)</label>
            <input
              v-model.number="defaults.deadzoneDegrees"
              type="range"
              min="0"
              max="5"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.deadzoneDegrees.toFixed(1) }}°</p>
            <p class="text-xs text-muted">Minimum head movement before cursor starts moving. Higher values reduce jitter.</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Max move per frame</label>
            <input
              v-model.number="defaults.maxMovePerFrame"
              type="range"
              min="50"
              max="500"
              step="10"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.maxMovePerFrame }}</p>
            <p class="text-xs text-muted">Maximum cursor displacement per frame to prevent overshooting.</p>
          </div>
        </div>

        <div class="mt-4 space-y-3">
          <ModuleBindingPanel
            heading="Toggle Binding"
            :binding="defaults.toggleBinding"
            hint="Press the assigned control to temporarily disable Head Cursor. Press again to re-enable."
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
                    ? 'Binding enables Head Cursor (disabled by default, press to activate).'
                    : 'Binding disables Head Cursor (enabled by default, press to suspend).' }}
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
          <p class="text-sm text-muted">Per-application settings for Head Cursor.</p>
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
        <strong>Broad enablement is active.</strong> Head Cursor applies to applications without a custom profile, including ones you have not validated. The safer setup is Default Off with this list used as an application allowlist.
      </div>

      <ProfileShell
        v-for="(profile, index) in config.profiles"
        :key="index"
        :index="index"
        :profile="profile"
        :applications="applications"
        module-label="Head Cursor"
        @remove="$emit('removeProfile', index)"
        @sync-name="$emit('syncProfileName', index)"
      >
        <div class="grid gap-4 md:grid-cols-2">
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Yaw sensitivity</label>
            <input
              v-model.number="profile.settings.yawSensitivity"
              type="range"
              min="0.1"
              max="5"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.yawSensitivity.toFixed(1) }}</p>
          </div>
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Pitch sensitivity</label>
            <input
              v-model.number="profile.settings.pitchSensitivity"
              type="range"
              min="0.1"
              max="5"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.pitchSensitivity.toFixed(1) }}</p>
          </div>
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Yaw multiplier</label>
            <input
              v-model.number="profile.settings.yawMultiplier"
              type="range"
              min="0.5"
              max="10"
              step="0.5"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.yawMultiplier.toFixed(1) }}</p>
          </div>
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Pitch multiplier</label>
            <input
              v-model.number="profile.settings.pitchMultiplier"
              type="range"
              min="0.5"
              max="10"
              step="0.5"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.pitchMultiplier.toFixed(1) }}</p>
          </div>
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Deadzone (degrees)</label>
            <input
              v-model.number="profile.settings.deadzoneDegrees"
              type="range"
              min="0"
              max="5"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.deadzoneDegrees.toFixed(1) }}°</p>
          </div>
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">Max move per frame</label>
            <input
              v-model.number="profile.settings.maxMovePerFrame"
              type="range"
              min="50"
              max="500"
              step="10"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.maxMovePerFrame }}</p>
          </div>
        </div>

        <div class="mt-3 space-y-3">
          <ModuleBindingPanel
            heading="Toggle Binding"
            :binding="profile.settings.toggleBinding"
            hint="Press the assigned control to temporarily disable Head Cursor. Press again to re-enable."
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
                    ? 'Binding enables Head Cursor (disabled by default, press to activate).'
                    : 'Binding disables Head Cursor (enabled by default, press to suspend).' }}
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
        No custom profiles yet. Add one when an application needs different Head Cursor settings from the default.
      </div>
    </section>
  </div>
</template>
