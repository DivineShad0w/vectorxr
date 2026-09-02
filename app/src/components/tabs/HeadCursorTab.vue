<script setup lang="ts">
import { computed, ref } from 'vue'

import ModuleBindingPage from '../ModuleBindingPage.vue'
import ModuleBindingPanel from '../ModuleBindingPanel.vue'
import ProfileShell from '../ProfileShell.vue'
import { t } from '../../i18n/index.ts'
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
    :label="t('headcursor.tab.binding.label')"
    :description="t('headcursor.tab.binding.description')"
    :none-text="t('headcursor.tab.binding.noneText')"
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
          {{ t('headcursor.tab.intro') }}
        </p>
      </div>

      <section>
        <div class="mb-3">
          <p class="eyebrow text-xs font-semibold uppercase tracking-[0.24em]">{{ t('headcursor.tab.defaultProfile') }}</p>
          <p class="mt-1 text-sm text-muted">{{ t('headcursor.tab.defaultSummary') }}</p>
        </div>

        <div class="rounded-[1rem] border p-4 surface-panel-soft">
          <div class="flex flex-wrap items-center justify-between gap-3">
            <div class="max-w-2xl">
              <h3 class="text-base font-semibold tracking-tight">{{ t('headcursor.tab.enableHeading') }}</h3>
              <p class="mt-1 text-sm leading-6 text-muted">
                {{ config.enabled ? t('headcursor.tab.enabledHint') : t('headcursor.tab.disabledHint') }}
              </p>
            </div>
            <label class="pill-toggle inline-flex items-center gap-3 rounded-full px-4 py-2 text-sm font-medium">
              <input v-model="config.enabled" class="h-4 w-4 accent-depthxr-copper" type="checkbox" />
              {{ t('common.default') }} {{ config.enabled ? t('common.on') : t('common.off') }}
            </label>
          </div>
        </div>

        <div class="mt-4 grid gap-4 md:grid-cols-2">
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.yawSensitivity') }}</label>
            <input
              v-model.number="defaults.yawSensitivity"
              type="range"
              min="0.1"
              max="10"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.yawSensitivity.toFixed(1) }}</p>
            <p class="text-xs text-muted">{{ t('headcursor.fields.yawSensitivityHint') }}</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.pitchSensitivity') }}</label>
            <input
              v-model.number="defaults.pitchSensitivity"
              type="range"
              min="0.1"
              max="10"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.pitchSensitivity.toFixed(1) }}</p>
            <p class="text-xs text-muted">{{ t('headcursor.fields.pitchSensitivityHint') }}</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.yawMultiplier') }}</label>
            <input
              v-model.number="defaults.yawMultiplier"
              type="range"
              min="0.5"
              max="10"
              step="0.5"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.yawMultiplier.toFixed(1) }}</p>
            <p class="text-xs text-muted">{{ t('headcursor.fields.yawMultiplierHint') }}</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.pitchMultiplier') }}</label>
            <input
              v-model.number="defaults.pitchMultiplier"
              type="range"
              min="0.5"
              max="10"
              step="0.5"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.pitchMultiplier.toFixed(1) }}</p>
            <p class="text-xs text-muted">{{ t('headcursor.fields.pitchMultiplierHint') }}</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.deadzone') }}</label>
            <input
              v-model.number="defaults.deadzoneDegrees"
              type="range"
              min="0"
              max="5"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.deadzoneDegrees.toFixed(1) }}°</p>
            <p class="text-xs text-muted">{{ t('headcursor.fields.deadzoneHint') }}</p>
          </div>

          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.maxMovePerFrame') }}</label>
            <input
              v-model.number="defaults.maxMovePerFrame"
              type="range"
              min="50"
              max="500"
              step="10"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ defaults.maxMovePerFrame }}</p>
            <p class="text-xs text-muted">{{ t('headcursor.fields.maxMovePerFrameHint') }}</p>
          </div>
        </div>

        <div class="mt-4 space-y-3">
          <ModuleBindingPanel
            :heading="t('headcursor.tab.binding.label')"
            :binding="defaults.toggleBinding"
            :hint="t('headcursor.tab.binding.description')"
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
                <span class="block text-sm font-medium">{{ t('headcursor.tab.inverted.label') }}</span>
                <span class="block text-sm leading-6 text-muted">
                  {{ defaults.invertedToggle ? t('headcursor.tab.inverted.onHint') : t('headcursor.tab.inverted.offHint') }}
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
          <h2 class="text-lg font-semibold tracking-tight">{{ t('headcursor.tab.profilesHeading') }}</h2>
          <p class="text-sm text-muted">{{ t('headcursor.tab.profilesSummary') }}</p>
        </div>
        <button
          class="button-accent rounded-[0.75rem] px-5 py-2.5 text-sm font-medium"
          type="button"
          @click="$emit('addProfile')"
        >
          {{ t('headcursor.tab.addProfile') }}
        </button>
      </div>

      <div v-if="config.enabled" class="rounded-[0.9rem] border px-4 py-3 text-sm leading-6 chip-warning" style="border-color: var(--app-border)">
        <strong>{{ t('headcursor.tab.broadEnablementLead') }}</strong> {{ t('headcursor.tab.broadEnablementBody') }}
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
            <label class="block text-sm font-medium">{{ t('headcursor.fields.yawSensitivity') }}</label>
            <input
              v-model.number="profile.settings.yawSensitivity"
              type="range"
              min="0.1"
              max="10"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.yawSensitivity.toFixed(1) }}</p>
          </div>
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.pitchSensitivity') }}</label>
            <input
              v-model.number="profile.settings.pitchSensitivity"
              type="range"
              min="0.1"
              max="10"
              step="0.1"
              class="mt-2 w-full accent-depthxr-copper"
            />
            <p class="mt-1 text-right text-sm text-muted">{{ profile.settings.pitchSensitivity.toFixed(1) }}</p>
          </div>
          <div class="rounded-[1rem] border p-4 surface-panel-soft">
            <label class="block text-sm font-medium">{{ t('headcursor.fields.yawMultiplier') }}</label>
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
            <label class="block text-sm font-medium">{{ t('headcursor.fields.pitchMultiplier') }}</label>
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
            <label class="block text-sm font-medium">{{ t('headcursor.fields.deadzone') }}</label>
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
            <label class="block text-sm font-medium">{{ t('headcursor.fields.maxMovePerFrame') }}</label>
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
            :heading="t('headcursor.tab.binding.label')"
            :binding="profile.settings.toggleBinding"
            :hint="t('headcursor.tab.binding.description')"
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
                <span class="block text-sm font-medium">{{ t('headcursor.tab.inverted.label') }}</span>
                <span class="block text-sm leading-6 text-muted">
                  {{ profile.settings.invertedToggle ? t('headcursor.tab.inverted.onHint') : t('headcursor.tab.inverted.offHint') }}
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
        {{ t('headcursor.tab.emptyProfiles') }}
      </div>
    </section>
  </div>
</template>
